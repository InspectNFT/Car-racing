// Copyright (c) 2020 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include <algorithm>
#include <cmath>
#include <deque>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "boost/geometry.hpp"
#include "boost/geometry/geometries/point_xy.hpp"
#include "boost/geometry/geometries/polygon.hpp"
#include "boost/pointer_cast.hpp"
#include "carla/client/ActorList.h"
#include "carla/client/Vehicle.h"
#include "carla/client/Walker.h"
#include "carla/client/World.h"
#include "carla/geom/Location.h"
#include "carla/geom/Math.h"
#include "carla/geom/Vector3D.h"
#include "carla/Logging.h"
#include "carla/rpc/ActorId.h"
#include "carla/rpc/TrafficLightState.h"

#include "carla/trafficmanager/MessengerAndDataTypes.h"
#include "carla/trafficmanager/Parameters.h"
#include "carla/trafficmanager/PipelineStage.h"

using IdPair = std::pair<uint32_t,uint32_t>;

//custom specialization of std::hash can be injected in namespace std
 namespace std 
 {
  template <>
  struct hash<IdPair>
  {
    std::size_t operator()(const IdPair& s) const noexcept
    {
      return std::hash<uint32_t>{}(s.first) ^ std::hash<uint32_t>{}(s.second);
    }

  };
} 


namespace carla {
namespace traffic_manager {

  namespace cc = carla::client;
  namespace cg = carla::geom;
  namespace chr = std::chrono;
  namespace bg = boost::geometry;

  using ActorId = carla::ActorId;
  using Actor = carla::SharedPtr<cc::Actor>;
  using Polygon = bg::model::polygon<bg::model::d2::point_xy<double>>;
  using LocationList = std::vector<cg::Location>;
  using SimpleWaypointPtr = std::shared_ptr<SimpleWaypoint>;
  using TLS = carla::rpc::TrafficLightState;
  

  /// Structure to hold the Geometry of refference vehicle to Other Vehicle 
  struct GeometryComparisonCache {

     double reference_vehicle_to_other_geodesic;
     double other_vehicle_to_reference_geodesic;
     double inter_geodesic_distance;
     double inter_bbox_distance;
};

struct LocationTuple {  // WIP - This is added only for debugging 

    cg::Location ref_loc,other_loc;
};

/// This class is the thread executable for the collision detection stage
/// and is responsible for checking possible collisions with other
/// cars along the vehicle's trajectory.
class CollisionStage : public PipelineStage
{

private:
  /// Geometry data for the vehicle
  std::unordered_map<std::size_t, GeometryComparisonCache> vehicle_cache;
  std::unordered_map<std::size_t, LocationTuple> vehicle_loc_cache; // WIP - This is added only for debugging 
  /// Selection key for switching between output frames.
  bool frame_selector;
  /// Pointer to data received from localization stage.
  std::shared_ptr<LocalizationToCollisionFrame> localization_frame;
  /// Pointers to output frames to be shared with motion planner stage.
  std::shared_ptr<CollisionToPlannerFrame> planner_frame_a;
  std::shared_ptr<CollisionToPlannerFrame> planner_frame_b;
  /// Pointers to messenger objects.
  std::shared_ptr<LocalizationToCollisionMessenger> localization_messenger;
  std::shared_ptr<CollisionToPlannerMessenger> planner_messenger;
  /// Runtime parameterization object.
  Parameters &parameters;
  /// Reference to Carla's debug helper object.
  cc::DebugHelper &debug_helper;
  /// The map used to connect actor ids to the array index of data frames.
  std::unordered_map<ActorId, uint64_t> vehicle_id_to_index;
  /// An object used to keep track of time between checking for all world
  /// actors.
  chr::time_point<chr::system_clock, chr::nanoseconds> last_world_actors_pass_instance;
  /// Number of vehicles registered with the traffic manager.
  uint64_t number_of_vehicles;
  /// Structure to hold the geodesic boundaries during one iteration.
  std::unordered_map<ActorId, LocationList> geodesic_boundaries;
  /// Snippet profiler for measuring execution time.
  SnippetProfiler snippet_profiler;

  /// Returns the bounding box corners of the vehicle passed to the method.
  LocationList GetBoundary(const Actor &actor, const cg::Location &location);

  /// Returns the extrapolated bounding box of the vehicle along its
  /// trajectory.
  LocationList GetGeodesicBoundary(const Actor &actor, const cg::Location &location);

  /// Method to construct a boost polygon object.
  Polygon GetPolygon(const LocationList &boundary);

  /// The method returns true if ego_vehicle should stop and wait for
  /// other_vehicle to pass.
  bool NegotiateCollision(const Actor &ego_vehicle, const Actor &other_vehicle,
                          const cg::Location &reference_location, const cg::Location &other_location,
                          const SimpleWaypointPtr& closest_point,
                          const SimpleWaypointPtr& junction_look_ahead);

  /// Method to calculate the speed dependent bounding box extention for a vehicle.
  float GetBoundingBoxExtention(const Actor &ego_vehicle);

  /// At intersections, used to see if there is space after the junction
  bool IsLocationAfterJunctionSafe(const Actor &ego_actor, const Actor &overlapped_actor,
                                   const SimpleWaypointPtr safe_point, const cg::Location &other_location);

  /// A simple method used to draw bounding boxes around vehicles
  void DrawBoundary(const LocationList &boundary);

  /// A method used compute Geometry result between two vehicle
  GeometryComparisonCache GetGeometryBetweenActors(const Actor &reference_vehicle, const Actor &other_vehicle,
                                                   const cg::Location &reference_location, const cg::Location &other_location);

public:

CollisionStage(
        std::string stage_name,
        std::shared_ptr<LocalizationToCollisionMessenger> localization_messenger,
        std::shared_ptr<CollisionToPlannerMessenger> planner_messenger,
        Parameters &parameters,
        cc::DebugHelper &debug_helper);

  ~CollisionStage();

  void DataReceiver() override;

  void Action() override;

  void DataSender() override;

  };

} // namespace traffic_manager
} // namespace carla
