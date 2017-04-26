// CARLA, Copyright (C) 2017 Computer Vision Center (CVC)

#pragma once

#include "PostProcessEffect.h"
#include "CameraDescription.generated.h"

USTRUCT()
struct FCameraDescription
{
  GENERATED_BODY()

  /** X size in pixels of the captured image. */
  UPROPERTY(Category = "Camera Description", EditDefaultsOnly, meta=(ClampMin = "1"))
  uint32 ImageSizeX = 720u;

  /** Y size in pixels of the captured image. */
  UPROPERTY(Category = "Camera Description", EditDefaultsOnly, meta=(ClampMin = "1"))
  uint32 ImageSizeY = 512u;

  /** Position relative to the player. */
  UPROPERTY(Category = "Camera Description", EditDefaultsOnly)
  FVector Position = {170.0f, 0.0f, 150.0f};

  /** Rotation relative to the player. */
  UPROPERTY(Category = "Camera Description", EditDefaultsOnly)
  FRotator Rotation = {0.0f, 0.0f, 0.0f};

  /** Post-process effect to be applied to the captured image. */
  UPROPERTY(Category = "Camera Description", EditDefaultsOnly)
  EPostProcessEffect PostProcessEffect = EPostProcessEffect::SceneFinal;

  /** Camera field of view (in degrees). */
  UPROPERTY(Category = "Camera Description", EditDefaultsOnly, meta=(DisplayName = "Field of View", ClampMin = "0.001", ClampMax = "360.0"))
  float FOVAngle = 90.0f;
};
