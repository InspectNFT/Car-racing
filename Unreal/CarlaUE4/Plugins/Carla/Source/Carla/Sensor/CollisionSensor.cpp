// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "Carla/Sensor/CollisionSensor.h"

#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Carla/Actor/ActorRegistry.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Game/CarlaEngine.h"
#include "Carla/Game/CarlaGameInstance.h"
#include "Carla/Game/CarlaGameModeBase.h"

ACollisionSensor::ACollisionSensor(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = false;
}

FActorDefinition ACollisionSensor::GetSensorDefinition()
{
  return UActorBlueprintFunctionLibrary::MakeGenericSensorDefinition(
      TEXT("other"),
      TEXT("collision"));
}

void ACollisionSensor::SetOwner(AActor *NewOwner)
{
  Super::SetOwner(NewOwner);

  /// @todo Deregister previous owner if there was one.

  if (NewOwner != nullptr)
  {
    NewOwner->OnActorHit.AddDynamic(this, &ACollisionSensor::OnCollisionEvent);
  }
}

void ACollisionSensor::OnCollisionEvent(
    AActor *Actor,
    AActor *OtherActor,
    FVector NormalImpulse,
    const FHitResult &Hit)
{
  if ((Actor == nullptr) || (OtherActor == nullptr)){
    return;
  }

  uint64_t CurrentFrame = FCarlaEngine::GetFrameCounter();
  auto CollisionRegistry_ = CollisionRegistry;
  for (auto & Collision: CollisionRegistry_){
    if (std::get<0>(Collision) < CurrentFrame){
      auto Index = std::find(CollisionRegistry.begin(), CollisionRegistry.end(), Collision);
      CollisionRegistry.erase(Index);
    }
    else if ((std::get<0>(Collision) == CurrentFrame) && (std::get<1>(Collision) == Actor) && (std::get<2>(Collision) == OtherActor)){
      return;
    }
  }

  const auto &Episode = GetEpisode();
  constexpr float TO_METERS = 1e-2;
  NormalImpulse *= TO_METERS;
  GetDataStream(*this).Send(
      *this,
      Episode.SerializeActor(Actor),
      Episode.SerializeActor(OtherActor),
      carla::geom::Vector3D{NormalImpulse.X, NormalImpulse.Y, NormalImpulse.Z});
  // record the collision event
  if (Episode.GetRecorder()->IsEnabled()){
    Episode.GetRecorder()->AddCollision(Actor, OtherActor);
  }

  CollisionRegistry.push_back(std::make_tuple(CurrentFrame, Actor, OtherActor));
}
