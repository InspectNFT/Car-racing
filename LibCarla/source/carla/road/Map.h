// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/NonCopyable.h"
#include "carla/geom/Transform.h"
#include "carla/road/MapData.h"
#include "carla/road/element/LaneMarking.h"
#include "carla/road/element/RoadInfoMarkRecord.h"
#include "carla/road/element/Waypoint.h"

#include <boost/optional.hpp>

#include <vector>

namespace carla {
namespace road {

  class Map : private MovableNonCopyable {
  public:

    /// @todo Don't define here.
    using RoadId = uint32_t;
    using LaneId = int32_t;
    using RoadDistance = float;

    using Waypoint = element::Waypoint;

    Map(MapData m)
      : _data(std::move(m)) {}

    std::vector<element::LaneMarking> CalculateCrossedLanes(
        const geom::Location &origin,
        const geom::Location &destination) const;

    const geom::GeoLocation &GetGeoReference() const {
      return _data.GetGeoReference();
    }

    /// ========================================================================
    /// -- Geometry ------------------------------------------------------------
    /// ========================================================================

    element::Waypoint GetClosestWaypointOnRoad(const geom::Location &location) const;

    boost::optional<element::Waypoint> GetWaypoint(const geom::Location &location) const;

    geom::Transform ComputeTransform(Waypoint waypoint) const;

    /// ========================================================================
    /// -- Road information ----------------------------------------------------
    /// ========================================================================

    const std::string &GetType(RoadId road_id, RoadDistance s) const;

    const std::string &GetType(Waypoint waypoint) const;

    double GetLaneWidth(Waypoint waypoint) const;

    bool IsIntersection(RoadId road_id) const;

    std::pair<element::RoadInfoMarkRecord *, element::RoadInfoMarkRecord *>
        GetMarkRecord(Waypoint waypoint) const;

    /// ========================================================================
    /// -- Waypoint generation -------------------------------------------------
    /// ========================================================================

    /// Return the list of waypoints placed at the entrance of each drivable
    /// successor lane; i.e., the list of each waypoint in the next road segment
    /// that a vehicle could drive from @a waypoint.
    std::vector<Waypoint> GetSuccessors(Waypoint waypoint) const;

    /// Return the list of waypoints at @a distance such that a vehicle at @a
    /// waypoint could drive to.
    std::vector<Waypoint> GetNext(Waypoint waypoint, RoadDistance distance) const;

    /// Return a waypoint at the lane of @a waypoint's right lane.
    boost::optional<Waypoint> GetRight(Waypoint waypoint) const;

    /// Return a waypoint at the lane of @a waypoint's left lane.
    boost::optional<Waypoint> GetLeft(Waypoint waypoint) const;

    /// Generate all the waypoints in @a map separated by @a approx_distance.
    std::vector<Waypoint> GenerateWaypoints(RoadDistance approx_distance) const;

    /// Returns a list of waypoints at the beginning of each lane of the map.
    // std::vector<Waypoint> GenerateLaneBegin() const;

    /// Returns a list of waypoints at the end of each lane of the map.
    // std::vector<Waypoint> GenerateLaneEnd() const;

    /// Generate the minimum set of waypoints that define the topology of @a
    /// map. The waypoints are placed at the entrance of each lane.
    std::vector<std::pair<Waypoint, Waypoint>> GenerateTopology() const;

  private:

    const Lane *GetLane(Waypoint waypoint) const;

    MapData _data;
  };

} // namespace road
} // namespace carla
