// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "Carla/Sensor/RayTraceLidar.h"

#include "DrawDebugHelpers.h"
#include "Engine/CollisionProfile.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "StaticMeshResources.h"

ARayTraceLidar::ARayTraceLidar(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true;

  auto MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
  MeshComp->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
  MeshComp->bHiddenInGame = true;
  MeshComp->CastShadow = false;
  MeshComp->PostPhysicsComponentTick.bCanEverTick = false;
  RootComponent = MeshComp;
}

void ARayTraceLidar::SetLidar(const FLidarDescription &LidarDescription)
{
  Description = LidarDescription;
  LidarMeasurement = FLidarMeasurement(Description.Channels);
  CreateLasers();
}

void ARayTraceLidar::CreateLasers()
{
  const auto NumberOfLasers = Description.Channels;
  check(NumberOfLasers > 0u);
  const float DeltaAngle = NumberOfLasers == 1u ? 0.f :
    (Description.UpperFovLimit - Description.LowerFovLimit) /
    static_cast<float>(NumberOfLasers - 1);
  LaserAngles.Empty(NumberOfLasers);
  for(auto i = 0u; i < NumberOfLasers; ++i)
  {
    const float VerticalAngle =
      Description.UpperFovLimit - static_cast<float>(i) * DeltaAngle;
    LaserAngles.Emplace(VerticalAngle);
  }
}

void ARayTraceLidar::Tick(const float DeltaTime)
{
  Super::Tick(DeltaTime);

  ReadPoints(DeltaTime);

  /// @todo Here we send the data.
  // auto &Stream = GetDataStream();
  // auto Buffer = Stream.PopBufferFromPool();
  // LidarMeasurement.CopyToBuffer(Buffer);
  // Stream.Send_GameThread(*this, std::move(Buffer));
}

void ARayTraceLidar::ReadPoints(const float DeltaTime)
{
  const uint32 ChannelCount = Description.Channels;
  const uint32 PointsToScanWithOneLaser =
    FMath::RoundHalfFromZero(
        Description.PointsPerSecond * DeltaTime / float(ChannelCount));

  if (PointsToScanWithOneLaser <= 0)
  {
    UE_LOG(
        LogCarla,
        Warning,
        TEXT("%s: no points requested this frame, try increasing the number of points per second."),
        *GetName());
    return;
  }

  check(ChannelCount == LaserAngles.Num());

  const float CurrentHorizontalAngle = LidarMeasurement.GetHorizontalAngle();
  const float AngleDistanceOfTick = Description.RotationFrequency * 360.0f * DeltaTime;
  const float AngleDistanceOfLaserMeasure = AngleDistanceOfTick / PointsToScanWithOneLaser;

  LidarMeasurement.Reset(ChannelCount * PointsToScanWithOneLaser);

  for (auto Channel = 0u; Channel < ChannelCount; ++Channel)
  {
    for (auto i = 0u; i < PointsToScanWithOneLaser; ++i)
    {
      FVector Point;
      const float Angle = CurrentHorizontalAngle + AngleDistanceOfLaserMeasure * i;
      if (ShootLaser(Channel, Angle, Point))
      {
        LidarMeasurement.WritePoint(Channel, Point);
      }
    }
  }

  const float HorizontalAngle = std::fmod(CurrentHorizontalAngle + AngleDistanceOfTick, 360.0f);
  LidarMeasurement.SetHorizontalAngle(HorizontalAngle);
}

bool ARayTraceLidar::ShootLaser(const uint32 Channel, const float HorizontalAngle, FVector &XYZ) const
{
  const float VerticalAngle = LaserAngles[Channel];

  FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Laser_Trace")), true, this);
  TraceParams.bTraceComplex = true;
  TraceParams.bReturnPhysicalMaterial = false;

  FHitResult HitInfo(ForceInit);

  FVector LidarBodyLoc = GetActorLocation();
  FRotator LidarBodyRot = GetActorRotation();
  FRotator LaserRot (VerticalAngle, HorizontalAngle, 0);  // float InPitch, float InYaw, float InRoll
  FRotator ResultRot = UKismetMathLibrary::ComposeRotators(
    LaserRot,
    LidarBodyRot
  );
  const auto Range = Description.Range;
  FVector EndTrace = Range * UKismetMathLibrary::GetForwardVector(ResultRot) + LidarBodyLoc;

  GetWorld()->LineTraceSingleByChannel(
    HitInfo,
    LidarBodyLoc,
    EndTrace,
    ECC_MAX,
    TraceParams,
    FCollisionResponseParams::DefaultResponseParam
  );

  if (HitInfo.bBlockingHit)
  {
    if (Description.ShowDebugPoints)
    {
      DrawDebugPoint(
        GetWorld(),
        HitInfo.ImpactPoint,
        10,  //size
        FColor(255,0,255),
        false,  //persistent (never goes away)
        0.1  //point leaves a trail on moving object
      );
    }

    XYZ = LidarBodyLoc - HitInfo.ImpactPoint;
    XYZ = UKismetMathLibrary::RotateAngleAxis(
      XYZ,
      - LidarBodyRot.Yaw + 90,
      FVector(0, 0, 1)
    );

    return true;
  } else {
    return false;
  }
}
