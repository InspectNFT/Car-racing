// CARLA, Copyright (C) 2017 Computer Vision Center (CVC)

#include "Carla.h"
#include "CarlaServer.h"

#include "GameFramework/PlayerStart.h"

#include "CarlaPlayerState.h"
#include "CarlaVehicleController.h"
#include "SceneCaptureCamera.h"
#include "Settings/CarlaSettings.h"

#include <carla/carla_server.h>

// =============================================================================
// -- Static local methods -----------------------------------------------------
// =============================================================================

static CarlaServer::ErrorCode ParseErrorCode(const uint32 ErrorCode)
{
  if (ErrorCode == CARLA_SERVER_SUCCESS) {
    return CarlaServer::Success;
  } else if (ErrorCode == CARLA_SERVER_TRY_AGAIN) {
    return CarlaServer::TryAgain;
  } else {
    return CarlaServer::Error;
  }
}

static int32 GetTimeOut(uint32 TimeOut, const bool bBlocking)
{
  return (bBlocking ? TimeOut : 0u);
}

// =============================================================================
// -- Set functions ------------------------------------------------------------
// =============================================================================

static inline void Set(float &lhs, float rhs)
{
  lhs = rhs;
}

static inline void Set(carla_vector3d &cVector, const FVector &uVector)
{
  cVector = {uVector.X, uVector.Y, uVector.Z};
}

static void Set(carla_image &cImage, const FCapturedImage &uImage)
{
  if (uImage.BitMap.Num() > 0) {
    cImage.width = uImage.SizeX;
    cImage.height = uImage.SizeY;
    cImage.type = PostProcessEffect::ToUInt(uImage.PostProcessEffect);
    cImage.data = &uImage.BitMap.GetData()->DWColor();

#ifdef CARLA_SERVER_EXTRA_LOG
    {
      auto GetImageType = [](carla::ImageType Type) {
        switch (Type) {
          case carla::IMAGE:        return TEXT("IMAGE");
          case carla::SCENE_FINAL:  return TEXT("SCENE_FINAL");
          case carla::DEPTH:        return TEXT("DEPTH");
          case carla::SEMANTIC_SEG: return TEXT("SEMANTIC_SEG");
          default:                  return TEXT("ERROR!");
        }
      };
      const auto Size = uImage.BitMap.Num();
      UE_LOG(LogCarlaServer, Log, TEXT("Sending image %dx%d (%d) %s"), cImage.width, cImage.height, Size, GetImageType(cImage.type()));
    }
  } else {
    UE_LOG(LogCarlaServer, Warning, TEXT("Sending empty image"));
#endif // CARLA_SERVER_EXTRA_LOG
  }
}

// =============================================================================
// -- CarlaServer --------------------------------------------------------------
// =============================================================================

CarlaServer::CarlaServer(const uint32 InWorldPort, const uint32 InTimeOut) :
  WorldPort(InWorldPort),
  TimeOut(InTimeOut),
  Server(carla_make_server()) {
  check(Server != nullptr);
}

CarlaServer::~CarlaServer()
{
  carla_free_server(Server);
}

CarlaServer::ErrorCode CarlaServer::Connect()
{
  UE_LOG(LogCarlaServer, Log, TEXT("Waiting for client to connect..."));
  return ParseErrorCode(carla_server_connect(Server, WorldPort, TimeOut));
}

CarlaServer::ErrorCode CarlaServer::ReadNewEpisode(UCarlaSettings &Settings, const bool bBlocking)
{
  carla_request_new_episode values;
  auto ec = ParseErrorCode(carla_read_request_new_episode(Server, values, GetTimeOut(TimeOut, bBlocking)));
  if (Success == ec) {
    auto IniFile = FString(values.ini_file_length, ANSI_TO_TCHAR(values.ini_file));
    UE_LOG(LogCarlaServer, Log, TEXT("Received new episode"));
#ifdef CARLA_SERVER_EXTRA_LOG
    UE_LOG(LogCarlaServer, Log, TEXT("Received CarlaSettings.ini:\n%s"), *IniFile);
#endif // CARLA_SERVER_EXTRA_LOG
    Settings.LoadSettingsFromString(IniFile);
  }
  return ec;
}

CarlaServer::ErrorCode CarlaServer::SendSceneDescription(
      const TArray<APlayerStart *> &AvailableStartSpots,
      const bool bBlocking)
{
  const int32 NumberOfStartPositions = AvailableStartSpots.Num();
  auto Positions = MakeUnique<carla_vector3d[]>(NumberOfStartPositions);

  for (auto i = 0u; i < NumberOfStartPositions; ++i) {
    Set(Positions[i], AvailableStartSpots[i]->GetActorLocation());
  }

  UE_LOG(LogCarlaServer, Log, TEXT("Sending %d available start positions"), NumberOfStartPositions);
  carla_scene_description scene;
  scene.player_start_locations = Positions.Get();
  scene.number_of_player_start_locations = NumberOfStartPositions;

  return ParseErrorCode(carla_write_scene_description(Server, scene, GetTimeOut(TimeOut, bBlocking)));
}

CarlaServer::ErrorCode CarlaServer::ReadEpisodeStart(uint32 &StartPositionIndex, const bool bBlocking)
{
  carla_episode_start values;
  auto ec = ParseErrorCode(carla_read_episode_start(Server, values, GetTimeOut(TimeOut, bBlocking)));
  if (Success == ec) {
    StartPositionIndex = values.player_start_location_index;
    UE_LOG(LogCarlaServer, Log, TEXT("Episode start received: { StartIndex = %d }"), StartPositionIndex);
  }
  return ec;
}

CarlaServer::ErrorCode CarlaServer::SendEpisodeReady(const bool bBlocking)
{
  UE_LOG(LogCarlaServer, Log, TEXT("Ready to play, notifying client"));
  const carla_episode_ready values = {true};
  return ParseErrorCode(carla_write_episode_ready(Server, values, GetTimeOut(TimeOut, bBlocking)));
}

CarlaServer::ErrorCode CarlaServer::ReadControl(ACarlaVehicleController &Player, const bool bBlocking)
{
  carla_control values;
  auto ec = ParseErrorCode(carla_read_control(Server, values, GetTimeOut(TimeOut, bBlocking)));
  if (Success == ec) {
    Player.SetSteeringInput(values.steer);
    Player.SetThrottleInput(values.throttle);
    Player.SetBrakeInput(values.brake);
    Player.SetHandbrakeInput(values.hand_brake);
    Player.SetReverse(values.reverse);
#ifdef CARLA_SERVER_EXTRA_LOG
    UE_LOG(
        LogCarlaServer,
        Log,
        TEXT("Read control: { Steer = %f, Throttle = %f, Brake = %f, Handbrake = %s, Reverse = %s }"),
        values.steer,
        values.throttle,
        values.brake,
        (values.hand_brake ? TEXT("True") : TEXT("False")),
        (values.reverse ? TEXT("True") : TEXT("False")));
#endif // CARLA_SERVER_EXTRA_LOG
  } else if ((!bBlocking) && (TryAgain == ec)) {
    UE_LOG(LogCarlaServer, Warning, TEXT("No control received from the client this frame!"));
  }
  return ec;
}

CarlaServer::ErrorCode CarlaServer::SendMeasurements(const ACarlaPlayerState &PlayerState)
{
  // Measurements.
  carla_measurements values;
  values.platform_timestamp = PlayerState.GetPlatformTimeStamp();
  values.game_timestamp = PlayerState.GetGameTimeStamp();
  auto &player = values.player_measurements;
  Set(player.location, PlayerState.GetLocation());
  Set(player.orientation, PlayerState.GetOrientation());
  Set(player.acceleration, PlayerState.GetAcceleration());
  Set(player.forward_speed, PlayerState.GetForwardSpeed());
  Set(player.collision_vehicles, PlayerState.GetCollisionIntensityCars());
  Set(player.collision_pedestrians, PlayerState.GetCollisionIntensityPedestrians());
  Set(player.collision_other, PlayerState.GetCollisionIntensityOther());
  Set(player.intersection_otherlane, PlayerState.GetOtherLaneIntersectionFactor());
  Set(player.intersection_offroad, PlayerState.GetOffRoadIntersectionFactor());

  // Images.
  const auto NumberOfImages = PlayerState.GetNumberOfImages();
  TUniquePtr<carla_image[]> images;
  if (NumberOfImages > 0) {
    images = MakeUnique<carla_image[]>(NumberOfImages);
    for (auto i = 0u; i < NumberOfImages; ++i) {
      Set(images[i], PlayerState.GetImages()[i]);
    }
  }

  return ParseErrorCode(carla_write_measurements(Server, values, images.Get(), NumberOfImages));
}
