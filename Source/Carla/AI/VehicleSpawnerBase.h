// CARLA, Copyright (C) 2017 Computer Vision Center (CVC)

#pragma once

#include "Util/ActorWithRandomEngine.h"
#include "VehicleSpawnerBase.generated.h"

class APlayerStart;

UCLASS(Abstract)
class CARLA_API AVehicleSpawnerBase : public AActorWithRandomEngine
{
  GENERATED_BODY()

public:

  // Sets default values for this actor's properties
  AVehicleSpawnerBase(const FObjectInitializer& ObjectInitializer);

protected:

  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  UFUNCTION(BlueprintImplementableEvent)
  void SpawnVehicle(const FTransform &SpawnTransform, AWheeledVehicle *&SpawnedCharacter);

  //UFUNCTION(BlueprintImplementableEvent)
  void TryToSpawnRandomVehicle();

  UFUNCTION(BlueprintImplementableEvent)
  AAICarlaVehicleController* GetVehicleController(AWheeledVehicle* Vechicle);

public:

   void SetNumberOfVehicles(int32 Count);

   APlayerStart* GetRandomSpawnPoint();

   void SpawnVehicleAtSpawnPoint(const APlayerStart &SpawnPoint);

protected:

/** If false, no walker will be spawned. */
  UPROPERTY(Category = "Vehicle Spawner", EditAnywhere)
  bool bSpawnVehicles = true;

  /** Number of walkers to be present within the volume. */
  UPROPERTY(Category = "Vehicle Spawner", EditAnywhere, meta = (EditCondition = bSpawnVehicles, ClampMin = "1"))
  int32 NumberOfVehicles = 10;

  UPROPERTY(Category = "Vechicle Spawner", VisibleAnywhere, AdvancedDisplay)
  TArray<APlayerStart *> SpawnPoints;

  UPROPERTY(Category = "Vehicle Spawner", BlueprintReadOnly, VisibleAnywhere, AdvancedDisplay)
  TArray<AWheeledVehicle *> Vehicles;
};
