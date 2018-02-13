// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "GameFramework/Actor.h"
#include "Sensor.generated.h"

/// Base class for sensors.
UCLASS(Abstract, hidecategories=(Collision, Attachment, Actor))
class CARLA_API ASensor : public AActor
{
  GENERATED_BODY()

public:

  ASensor(const FObjectInitializer& ObjectInitializer);

  uint32 GetId() const
  {
    return Id;
  }

  void SetName(FString InName)
  {
    Name = InName;
  }

  const FString &GetName() const
  {
    return Name;
  }

  void AttachToActor(AActor *Actor);

private:

  UPROPERTY(VisibleAnywhere)
  uint32 Id;

  UPROPERTY(VisibleAnywhere)
  FString Name;
};
