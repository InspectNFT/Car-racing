// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "Carla/Recorder/CarlaReplayerHelper.h"

#include "Carla/Actor/ActorDescription.h"
#include "Carla/Actor/CarlaActor.h"
#include "Carla/Vehicle/WheeledVehicleAIController.h"
#include "Carla/Walker/WalkerControl.h"
#include "Carla/Walker/WalkerController.h"
#include "Carla/Lights/CarlaLight.h"
#include "Carla/Lights/CarlaLightSubsystem.h"
#include "Carla/Actor/ActorSpawnResult.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Traffic/TrafficSignBase.h"
#include "Carla/Traffic/TrafficLightBase.h"
#include "Carla/Vehicle/CarlaWheeledVehicle.h"
#include "Engine/StaticMeshActor.h"

#include <compiler/disable-ue4-macros.h>
#include <carla/rpc/VehicleLightState.h>
#include <compiler/enable-ue4-macros.h>


#include "EngineUtils.h"

// create or reuse an actor for replaying
std::pair<int, FCarlaActor*>CarlaReplayerHelper::TryToCreateReplayerActor(
    FVector &Location,
    FVector &Rotation,
    FActorDescription &ActorDesc,
    uint32_t DesiredId,
    bool SpawnSensors)
{
  check(Episode != nullptr);

  FCarlaActor view_empty;

  // check type of actor we need
  if (ActorDesc.Id.StartsWith("traffic."))
  {
    AActor *Actor = FindTrafficLightAt(Location);
    if (Actor != nullptr)
    {
      // actor found
      FCarlaActor* CarlaActor = Episode->FindCarlaActor(Actor);
      // reuse that actor
      return std::pair<int, FCarlaActor*>(2, CarlaActor);
    }
    else
    {
      // actor not found
      UE_LOG(LogCarla, Log, TEXT("TrafficLight not found"));
      return std::pair<int, FCarlaActor*>(0, nullptr);
    }
  }
  else if (SpawnSensors || !ActorDesc.Id.StartsWith("sensor."))
  {
    // check if an actor of that type already exist with same id
    if (Episode->GetActorRegistry().Contains(DesiredId))
    {
      auto* CarlaActor = Episode->FindCarlaActor(DesiredId);
      const FActorDescription *desc = &CarlaActor->GetActorInfo()->Description;
      if (desc->Id == ActorDesc.Id)
      {
        // we don't need to create, actor of same type already exist
        // relocate
        FRotator Rot = FRotator::MakeFromEuler(Rotation);
        FTransform Trans2(Rot, Location, FVector(1, 1, 1));
        CarlaActor->GetActor()->SetActorTransform(Trans2, false, nullptr, ETeleportType::TeleportPhysics);
        return std::pair<int, FCarlaActor*>(2, CarlaActor);
      }
    }
    // create the transform
    FRotator Rot = FRotator::MakeFromEuler(Rotation);
    FTransform Trans(Rot, FVector(0, 0, 100000), FVector(1, 1, 1));
    // create as new actor
    TPair<EActorSpawnResultStatus, FCarlaActor*> Result = Episode->SpawnActorWithInfo(Trans, ActorDesc, DesiredId);
    if (Result.Key == EActorSpawnResultStatus::Success)
    {
      // relocate
      FTransform Trans2(Rot, Location, FVector(1, 1, 1));
      Result.Value->GetActor()->SetActorTransform(Trans2, false, nullptr, ETeleportType::TeleportPhysics);
      return std::pair<int, FCarlaActor*>(1, Result.Value);
    }
    else
    {
      UE_LOG(LogCarla, Log, TEXT("Actor could't be created by replayer"));
      return std::pair<int, FCarlaActor*>(0, Result.Value);
    }
  }
  else
  {
    // actor ignored
    return std::pair<int, FCarlaActor*>(0, nullptr);
  }
}

AActor *CarlaReplayerHelper::FindTrafficLightAt(FVector Location)
{
  check(Episode != nullptr);
  auto World = Episode->GetWorld();
  check(World != nullptr);

  // get its position (truncated as int's)
  int x = static_cast<int>(Location.X);
  int y = static_cast<int>(Location.Y);
  int z = static_cast<int>(Location.Z);

  // search an "traffic." actor at that position
  for (TActorIterator<ATrafficSignBase> It(World); It; ++It)
  {
    ATrafficSignBase *Actor = *It;
    check(Actor != nullptr);
    FVector vec = Actor->GetTransform().GetTranslation();
    int x2 = static_cast<int>(vec.X);
    int y2 = static_cast<int>(vec.Y);
    int z2 = static_cast<int>(vec.Z);
    if ((x2 == x) && (y2 == y) && (z2 == z))
    {
      // actor found
      return static_cast<AActor *>(Actor);
    }
  }
  // actor not found
  return nullptr;
}

// enable / disable physics for an actor
bool CarlaReplayerHelper::SetActorSimulatePhysics(const FCarlaActor* CarlaActor, bool bEnabled)
{
  if (!CarlaActor)
  {
    return false;
  }
  auto RootComponent = Cast<UPrimitiveComponent>(CarlaActor->GetActor()->GetRootComponent());
  if (RootComponent == nullptr)
  {
    return false;
  }
  RootComponent->SetSimulatePhysics(bEnabled);

  return true;
}

// enable / disable autopilot for an actor
bool CarlaReplayerHelper::SetActorAutopilot(const FCarlaActor* CarlaActor, bool bEnabled, bool bKeepState)
{
  if (!CarlaActor)
  {
    return false;
  }
  auto Vehicle = Cast<ACarlaWheeledVehicle>(CarlaActor->GetActor());
  if (Vehicle == nullptr)
  {
    return false;
  }
  auto Controller = Cast<AWheeledVehicleAIController>(Vehicle->GetController());
  if (Controller == nullptr)
  {
    return false;
  }
  Controller->SetAutopilot(bEnabled, bKeepState);

  return true;
}

// replay event for creating actor
std::pair<int, uint32_t> CarlaReplayerHelper::ProcessReplayerEventAdd(
    FVector Location,
    FVector Rotation,
    CarlaRecorderActorDescription Description,
    uint32_t DesiredId,
    bool bIgnoreHero,
    bool ReplaySensors)
{
  check(Episode != nullptr);
  FActorDescription ActorDesc;
  bool IsHero = false;

  // prepare actor description
  ActorDesc.UId = Description.UId;
  ActorDesc.Id = Description.Id;
  for (const auto &Item : Description.Attributes)
  {
    FActorAttribute Attr;
    Attr.Type = static_cast<EActorAttributeType>(Item.Type);
    Attr.Id = Item.Id;
    Attr.Value = Item.Value;
    ActorDesc.Variations.Add(Attr.Id, std::move(Attr));
    // check for hero
    if (Item.Id == "role_name" && Item.Value == "hero")
      IsHero = true;
  }

  auto result = TryToCreateReplayerActor(
      Location,
      Rotation,
      ActorDesc,
      DesiredId,
      ReplaySensors);

  if (result.first != 0)
  {
    // disable physics and autopilot on vehicles
    if (result.second->GetActorType() == FCarlaActor::ActorType::Vehicle)
    {
      // ignore hero ?
      if (!(bIgnoreHero && IsHero))
      {
        // disable physics
        SetActorSimulatePhysics(result.second, false);
        // disable autopilot
        SetActorAutopilot(result.second, false, false);
      }
      else
      {
        // reenable physics just in case
        SetActorSimulatePhysics(result.second, true);
      }
    }
  }

  return std::make_pair(result.first, result.second->GetActorId());
}

// replay event for removing actor
bool CarlaReplayerHelper::ProcessReplayerEventDel(uint32_t DatabaseId)
{
  check(Episode != nullptr);
  auto actor = Episode->FindCarlaActor(DatabaseId)->GetActor();
  if (actor == nullptr)
  {
    UE_LOG(LogCarla, Log, TEXT("Actor %d not found to destroy"), DatabaseId);
    return false;
  }
  Episode->DestroyActor(actor);
  return true;
}

// replay event for parenting actors
bool CarlaReplayerHelper::ProcessReplayerEventParent(uint32_t ChildId, uint32_t ParentId)
{
  check(Episode != nullptr);
  AActor *child = Episode->FindCarlaActor(ChildId)->GetActor();
  AActor *parent = Episode->FindCarlaActor(ParentId)->GetActor();
  if (child && parent)
  {
    child->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
    child->SetOwner(parent);
    return true;
  }
  else
  {
    UE_LOG(LogCarla, Log, TEXT("Parenting Actors not found"));
    return false;
  }
}

// reposition actors
bool CarlaReplayerHelper::ProcessReplayerPosition(CarlaRecorderPosition Pos1, CarlaRecorderPosition Pos2, double Per, double DeltaTime)
{
  check(Episode != nullptr);
  AActor *Actor = Episode->FindCarlaActor(Pos1.DatabaseId)->GetActor();
  FVector Location;
  FRotator Rotation;
  if (Actor  && !Actor->IsPendingKill())
  {
    // check to assign first position or interpolate between both
    if (Per == 0.0)
    {
      // assign position 1
      Location = FVector(Pos1.Location);
      Rotation = FRotator::MakeFromEuler(Pos1.Rotation);
    }
    else
    {
      // interpolate positions
      Location = FMath::Lerp(FVector(Pos1.Location), FVector(Pos2.Location), Per);
      Rotation = FMath::Lerp(FRotator::MakeFromEuler(Pos1.Rotation), FRotator::MakeFromEuler(Pos2.Rotation), Per);
    }
    // set new transform
    FTransform Trans(Rotation, Location, FVector(1, 1, 1));
    Actor->SetActorTransform(Trans, false, nullptr, ETeleportType::None);
    return true;
  }
  return false;
}

// reposition the camera
bool CarlaReplayerHelper::SetCameraPosition(uint32_t Id, FVector Offset, FQuat Rotation)
{
  check(Episode != nullptr);

  // get specator pawn
  APawn *Spectator = Episode->GetSpectatorPawn();
  // get the actor to follow
  AActor *Actor = Episode->FindCarlaActor(Id)->GetActor();

  // check
  if (!Spectator || !Actor)
   return false;

  // set the new position
  FQuat ActorRot = Actor->GetActorTransform().GetRotation();
  FVector Pos = Actor->GetActorTransform().GetTranslation() + (ActorRot.RotateVector(Offset));
  Spectator->SetActorLocation(Pos);
  Spectator->SetActorRotation(ActorRot * Rotation);

  return true;
}

bool CarlaReplayerHelper::ProcessReplayerStateTrafficLight(CarlaRecorderStateTrafficLight State)
{
  check(Episode != nullptr);
  AActor *Actor = Episode->FindCarlaActor(State.DatabaseId)->GetActor();
  if (Actor && !Actor->IsPendingKill())
  {
    auto TrafficLight = Cast<ATrafficLightBase>(Actor);
    if (TrafficLight != nullptr)
    {
      TrafficLight->SetTrafficLightState(static_cast<ETrafficLightState>(State.State));
      TrafficLight->SetTimeIsFrozen(State.IsFrozen);
      TrafficLight->SetElapsedTime(State.ElapsedTime);
    }
    return true;
  }
  return false;
}

// set the animation for Vehicles
void CarlaReplayerHelper::ProcessReplayerAnimVehicle(CarlaRecorderAnimVehicle Vehicle)
{
  check(Episode != nullptr);
  AActor *Actor = Episode->FindCarlaActor(Vehicle.DatabaseId)->GetActor();
  if (Actor && !Actor->IsPendingKill())
  {
    auto Veh = Cast<ACarlaWheeledVehicle>(Actor);
    if (Veh == nullptr)
    {
      return;
    }

    FVehicleControl Control;
    Control.Throttle = Vehicle.Throttle;
    Control.Steer = Vehicle.Steering;
    Control.Brake = Vehicle.Brake;
    Control.bHandBrake = Vehicle.bHandbrake;
    Control.bReverse = (Vehicle.Gear < 0);
    Control.Gear = Vehicle.Gear;
    Control.bManualGearShift = false;
    Veh->ApplyVehicleControl(Control, EVehicleInputPriority::User);
  }
}

// set the lights for vehicles
void CarlaReplayerHelper::ProcessReplayerLightVehicle(CarlaRecorderLightVehicle LightVehicle)
{
  check(Episode != nullptr);
  AActor *Actor = Episode->FindCarlaActor(LightVehicle.DatabaseId)->GetActor();
  if (Actor && !Actor->IsPendingKill())
  {
    auto Veh = Cast<ACarlaWheeledVehicle>(Actor);
    if (Veh == nullptr)
    {
      return;
    }

    carla::rpc::VehicleLightState LightState(LightVehicle.State);
    Veh->SetVehicleLightState(FVehicleLightState(LightState));
  }
}

void CarlaReplayerHelper::ProcessReplayerLightScene(CarlaRecorderLightScene LightScene)
{
  check(Episode != nullptr);
  UWorld* World = Episode->GetWorld();
  if(World)
  {
    UCarlaLightSubsystem* CarlaLightSubsystem = World->GetSubsystem<UCarlaLightSubsystem>();
    if (!CarlaLightSubsystem)
    {
      return;
    }
    auto* CarlaLight = CarlaLightSubsystem->GetLight(LightScene.LightId);
    if (CarlaLight)
    {
      CarlaLight->SetLightIntensity(LightScene.Intensity);
      CarlaLight->SetLightColor(LightScene.Color);
      CarlaLight->SetLightOn(LightScene.bOn);
      CarlaLight->SetLightType(static_cast<ELightType>(LightScene.Type));
    }
  }
}

// set the animation for walkers
void CarlaReplayerHelper::ProcessReplayerAnimWalker(CarlaRecorderAnimWalker Walker)
{
  SetWalkerSpeed(Walker.DatabaseId, Walker.Speed);
}

// replay finish
bool CarlaReplayerHelper::ProcessReplayerFinish(bool bApplyAutopilot, bool bIgnoreHero, std::unordered_map<uint32_t, bool> &IsHero)
{
  // set autopilot and physics to all AI vehicles
  const FActorRegistry& Registry = Episode->GetActorRegistry();
  for (auto& It : Registry)
  {
    const FCarlaActor* CarlaActor = It.Value.Get();

    // enable physics only on vehicles
    switch (CarlaActor->GetActorType())
    {

      // vehicles
      case FCarlaActor::ActorType::Vehicle:
        // check for hero
        if (!(bIgnoreHero && IsHero[CarlaActor->GetActorId()]))
        {
            // stop all vehicles
            SetActorSimulatePhysics(CarlaActor, true);
            SetActorVelocity(CarlaActor, FVector(0, 0, 0));
            // reset any control assigned
            auto Veh = Cast<ACarlaWheeledVehicle>(const_cast<AActor *>(CarlaActor->GetActor()));
            if (Veh != nullptr)
            {
              FVehicleControl Control;
              Control.Throttle = 0.0f;
              Control.Steer = 0.0f;
              Control.Brake = 0.0f;
              Control.bHandBrake = false;
              Control.bReverse = false;
              Control.Gear = 1;
              Control.bManualGearShift = false;
              Veh->ApplyVehicleControl(Control, EVehicleInputPriority::User);
            }

        }
        break;

      // walkers
      case FCarlaActor::ActorType::Walker:
        // stop walker
        SetWalkerSpeed(CarlaActor->GetActorId(), 0.0f);
        break;
    }
  }
  return true;
}

void CarlaReplayerHelper::SetActorVelocity(const FCarlaActor *CarlaActor, FVector Velocity)
{
  if (!CarlaActor)
  {
    return;
  }
  auto RootComponent = Cast<UPrimitiveComponent>(CarlaActor->GetActor()->GetRootComponent());
  if (RootComponent == nullptr)
  {
    return;
  }
  RootComponent->SetPhysicsLinearVelocity(
      Velocity,
      false,
      "None");
}

// set the animation speed for walkers
void CarlaReplayerHelper::SetWalkerSpeed(uint32_t ActorId, float Speed)
{
  check(Episode != nullptr);
  FCarlaActor * CarlaActor = Episode->FindCarlaActor(ActorId);
  AActor *Actor = nullptr;
  if (CarlaActor)
  {
    Actor = CarlaActor->GetActor();
  }
  if (Actor && !Actor->IsPendingKill())
  {
    auto Wal = Cast<APawn>(Actor);
    if (Wal)
    {
      auto Controller = Cast<AWalkerController>(Wal->GetController());
      if (Controller != nullptr)
      {
        FWalkerControl Control;
        Control.Speed = Speed;
        Controller->ApplyWalkerControl(Control);
     }
    }
  }
}

void CarlaReplayerHelper::RemoveStaticProps()
{
  check(Episode != nullptr);
  auto World = Episode->GetWorld();
  for (TActorIterator<AStaticMeshActor> It(World); It; ++It)
  {
    auto Actor = *It;
    check(Actor != nullptr);
    auto MeshComponent = Actor->GetStaticMeshComponent();
    check(MeshComponent != nullptr);
    if (MeshComponent->Mobility == EComponentMobility::Movable)
    {
      Actor->Destroy();
    }
  }
}
