// CARLA, Copyright (C) 2017 Computer Vision Center (CVC)

#pragma once

#include "GameFramework/PlayerController.h"
#include "CarlaVehicleController.generated.h"

class ACarlaHUD;
class ACarlaPlayerState;
class ASceneCaptureCamera;
class UCameraComponent;
class USpringArmComponent;
class UWheeledVehicleMovementComponent;
struct FCameraDescription;

/**
 *
 */
UCLASS()
class CARLA_API ACarlaVehicleController : public APlayerController
{
  GENERATED_BODY()

  // ===========================================================================
  /// @name Constructor and destructor
  // ===========================================================================
  /// @{
public:

  ACarlaVehicleController();

  ~ACarlaVehicleController();

  /// @}
  // ===========================================================================
  /// @name APlayerController overrides
  // ===========================================================================
  /// @{
public:

  virtual void SetupInputComponent() override;

  virtual void Possess(APawn *aPawn) override;

  virtual void BeginPlay() override;

  virtual void Tick(float DeltaTime) override;

  virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;

  /// @}
  // ===========================================================================
  /// @name Vehicle pawn info
  // ===========================================================================
  /// @{
public:

  bool IsPossessingAVehicle() const
  {
    return MovementComponent != nullptr;
  }

  /// World location of the vehicle.
  FVector GetVehicleLocation() const;

  /// Speed forward in km/h. Might be negative if goes backwards.
  float GetVehicleForwardSpeed() const;

  /// Orientation vector of the vehicle, pointing forward.
  FVector GetVehicleOrientation() const;

  int32 GetVehicleCurrentGear() const;

  const ACarlaPlayerState &GetPlayerState() const
  {
    return *CarlaPlayerState;
  }

  /// @}
  // ===========================================================================
  /// @name Scene Capture
  // ===========================================================================
  /// @{
public:

  void AddSceneCaptureCamera(const FCameraDescription &CameraDescription);

  /// @}
  // ===========================================================================
  /// @name Vehicle movement
  // ===========================================================================
  /// @{
public:

  void SetThrottleInput(float Value);

  void SetSteeringInput(float Value);

  void SetHandbrakeInput(bool Value);

  void HoldHandbrake()
  {
    SetHandbrakeInput(true);
  }

  void ReleaseHandbrake()
  {
    SetHandbrakeInput(false);
  }

  /// @}
  // ===========================================================================
  /// @name Manual mode
  // ===========================================================================
  /// @{
public:

  bool IsInManualMode() const
  {
    return bManualMode;
  }

  void SetManualMode(bool On);

  void ToggleManualMode();

  /// @}
  // ===========================================================================
  /// @name Events
  // ===========================================================================
  /// @{
private:

  UFUNCTION()
  void OnCollisionEvent(
      AActor* Actor,
      AActor* OtherActor,
      FVector NormalImpulse,
      const FHitResult& Hit);

  /// @}
  // ===========================================================================
  /// @name Camera input
  // ===========================================================================
  /// @{
private:

  void ChangeCameraZoom(float Value);

  void ChangeCameraUp(float Value);

  void ChangeCameraRight(float Value);

  /// @}
  // ===========================================================================
  /// @name Input bindings
  // ===========================================================================
  /// @{
private:

  void SetupControllerInput();

  /// @}
  // ===========================================================================
  // -- Member variables -------------------------------------------------------
  // ===========================================================================
private:

  UPROPERTY()
  bool bManualMode = false;

  UPROPERTY()
  USpringArmComponent *SpringArm;

  UPROPERTY()
  UCameraComponent *PlayerCamera;

  UPROPERTY()
  UWheeledVehicleMovementComponent *MovementComponent;

  UPROPERTY()
  TArray<ASceneCaptureCamera *> SceneCaptureCameras;

  // Cast for quick access to the custom player state.
  UPROPERTY()
  ACarlaPlayerState *CarlaPlayerState;

  // Cast for quick access to the custom HUD.
  UPROPERTY()
  ACarlaHUD *CarlaHUD;
};
