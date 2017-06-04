// CARLA, Copyright (C) 2017 Computer Vision Center (CVC)

#pragma once

#include "GameFramework/Actor.h"
#include "StaticMeshResources.h"
#include "Game/CameraDescription.h"
#include "SceneCaptureCamera.generated.h"

class UDrawFrustumComponent;
class USceneCaptureComponent2D;
class UStaticMeshComponent;
class UTextureRenderTarget2D;

/// Own SceneCapture, re-implementing some of the methods since ASceneCapture
/// cannot be subclassed.
UCLASS(hidecategories=(Collision, Attachment, Actor))
class CARLA_API ASceneCaptureCamera : public AActor
{
  GENERATED_BODY()

public:

  ASceneCaptureCamera(const FObjectInitializer& ObjectInitializer);

protected:

  virtual void PostActorCreated() override;

public:

  virtual void BeginPlay() override;

  uint32 GetImageSizeX() const
  {
    return SizeX;
  }

  uint32 GetImageSizeY() const
  {
    return SizeY;
  }

  EPostProcessEffect GetPostProcessEffect() const
  {
    return PostProcessEffect;
  }

  void SetImageSize(uint32 SizeX, uint32 SizeY);

  void SetPostProcessEffect(EPostProcessEffect PostProcessEffect);

  void SetFOVAngle(float FOVAngle);

  void Set(const FCameraDescription &CameraDescription);

  void Set(
      const FCameraDescription &CameraDescription,
      const FCameraPostProcessParameters &OverridePostProcessParameters);

  bool ReadPixels(TArray<FColor> &BitMap) const;

private:

  /// Used to synchronize the DrawFrustumComponent with the
  /// SceneCaptureComponent2D settings.
  void UpdateDrawFrustum();

  UPROPERTY(Category = SceneCapture, EditAnywhere)
  uint32 SizeX;

  UPROPERTY(Category = SceneCapture, EditAnywhere)
  uint32 SizeY;

  UPROPERTY(Category = SceneCapture, EditAnywhere)
  EPostProcessEffect PostProcessEffect;

  /** To display the 3d camera in the editor. */
  UPROPERTY()
  UStaticMeshComponent* MeshComp;

  /** To allow drawing the camera frustum in the editor. */
  UPROPERTY()
  UDrawFrustumComponent* DrawFrustum;

  /** Render target necessary for scene capture */
  UPROPERTY(Transient)
  UTextureRenderTarget2D* CaptureRenderTarget;

  /** Scene capture component. */
  UPROPERTY(EditAnywhere)
  USceneCaptureComponent2D* CaptureComponent2D;

  UPROPERTY()
  UMaterial *PostProcessDepth;

  UPROPERTY()
  UMaterial *PostProcessSemanticSegmentation;
};
