// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Game/CarlaGameInstance.h"
#include "Carla/Game/TheNewCarlaGameModeBase.h"
#include "Carla/Settings/CarlaSettings.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "CarlaStatics.generated.h"

// =============================================================================
// -- UCarlaStatics declaration ------------------------------------------------
// =============================================================================

UCLASS()
class CARLA_API UCarlaStatics : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:

  UFUNCTION(BlueprintPure, Category="CARLA", meta=(WorldContext="WorldContextObject"))
  static ATheNewCarlaGameModeBase *GetGameMode(const UObject *WorldContextObject);

  UFUNCTION(BlueprintPure, Category="CARLA", meta=(WorldContext="WorldContextObject"))
  static UCarlaGameInstance *GetGameInstance(const UObject *WorldContextObject);

  UFUNCTION(BlueprintPure, Category="CARLA", meta=(WorldContext="WorldContextObject"))
  static UCarlaEpisode *GetCurrentEpisode(const UObject *WorldContextObject);

  UFUNCTION(BlueprintPure, Category="CARLA", meta=(WorldContext="WorldContextObject"))
  static UCarlaSettings *GetCarlaSettings(const UObject *WorldContextObject);

  UFUNCTION(BlueprintPure, Category="CARLA", meta=(WorldContext="WorldContextObject"))
  static FORCEINLINE TArray<FString> GetAllMapNames()
	{
		TArray<FString> TmpStrList, MapNameList;
    IFileManager::Get().FindFilesRecursive(MapNameList, *FPaths::ProjectContentDir(), TEXT("*.umap"), true, false, false);
    for (int i = 0; i < MapNameList.Num(); i++) {
        MapNameList[i].ParseIntoArray(TmpStrList, TEXT("Content/"), true);
        MapNameList[i] = TmpStrList[1];
        MapNameList[i] = MapNameList[i].Replace(TEXT(".umap"), TEXT(""));
        MapNameList[i] = "/Game/" + MapNameList[i];
    }
     return MapNameList;
	}
};

// =============================================================================
// -- UCarlaStatics implementation ---------------------------------------------
// =============================================================================

inline ATheNewCarlaGameModeBase *UCarlaStatics::GetGameMode(const UObject *WorldContext)
{
  return Cast<ATheNewCarlaGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
}

inline UCarlaGameInstance *UCarlaStatics::GetGameInstance(const UObject *WorldContext)
{
  return Cast<UCarlaGameInstance>(UGameplayStatics::GetGameInstance(WorldContext));
}

inline UCarlaEpisode *UCarlaStatics::GetCurrentEpisode(const UObject *WorldContext)
{
  auto GameInstance = GetGameInstance(WorldContext);
  return GameInstance != nullptr ? GameInstance->GetCarlaEpisode() : nullptr;
}

inline UCarlaSettings *UCarlaStatics::GetCarlaSettings(const UObject *WorldContext)
{
  auto GameInstance = GetGameInstance(WorldContext);
  return GameInstance != nullptr ? GameInstance->GetCARLASettings() : nullptr;
}

