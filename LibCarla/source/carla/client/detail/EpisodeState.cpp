// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "carla/client/detail/EpisodeState.h"

namespace carla {
namespace client {
namespace detail {

  static auto DeriveAcceleration(
      double delta_seconds,
      const geom::Vector3D &v0,
      const geom::Vector3D &v1) {
    /// @todo add methods to Vector3D for scalar multiplication.
    auto acc = v1 - v0;
    acc.x /= delta_seconds;
    acc.y /= delta_seconds;
    acc.z /= delta_seconds;
    return acc;
  }

  std::shared_ptr<const EpisodeState> EpisodeState::DeriveNextStep(
      const sensor::data::RawEpisodeState &state) const {
    auto next = std::make_shared<EpisodeState>();
    next->_frame_number = state.GetFrameNumber();
    next->_game_timestamp = state.GetGameTimeStamp();
    next->_platform_timestamp = state.GetPlatformTimeStamp();
    const auto delta_time = next->_game_timestamp - _game_timestamp;
    next->_actors.reserve(state.size());
    for (auto &&actor : state) {
      auto acceleration = DeriveAcceleration(
          delta_time,
          GetActorState(actor.id).velocity,
          actor.velocity);
      DEBUG_ONLY(auto result = )
      next->_actors.emplace(
          actor.id,
          ActorState{actor.transform, actor.velocity, acceleration});
      DEBUG_ASSERT(result.second);
    }
    return next;
  }

} // namespace detail
} // namespace client
} // namespace carla
