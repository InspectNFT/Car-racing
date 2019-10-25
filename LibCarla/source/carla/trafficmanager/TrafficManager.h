#pragma once

#include <algorithm>
#include <memory>
#include <random>
#include <unordered_set>
#include <vector>

#include "carla/client/Actor.h"
#include "carla/client/BlueprintLibrary.h"
#include "carla/client/Map.h"
#include "carla/client/World.h"
#include "carla/geom/Transform.h"
#include "carla/Logging.h"
#include "carla/Memory.h"

#include "carla/trafficmanager/AtomicActorSet.h"
#include "carla/trafficmanager/AtomicMap.h"
#include "carla/trafficmanager/BatchControlStage.h"
#include "carla/trafficmanager/CarlaDataAccessLayer.h"
#include "carla/trafficmanager/CollisionStage.h"
#include "carla/trafficmanager/InMemoryMap.h"
#include "carla/trafficmanager/LocalizationStage.h"
#include "carla/trafficmanager/MotionPlannerStage.h"
#include "carla/trafficmanager/TrafficLightStage.h"

namespace traffic_manager {

namespace cc = carla::client;

  using ActorPtr = carla::SharedPtr<cc::Actor>;

  /// The function of this class is to integrate all the various stages of
  /// the traffic manager appropriately using messengers.
  class TrafficManager {

  private:

    /// PID controller parameters.
    std::vector<float> longitudinal_PID_parameters;
    std::vector<float> longitudinal_highway_PID_parameters;
    std::vector<float> lateral_PID_parameters;
    /// Set of all actors registered with traffic manager.
    AtomicActorSet registered_actors;
    /// Pointer to local map cache.
    std::shared_ptr<InMemoryMap> local_map;
    /// Carla's client connection object.
    cc::Client client_connection;
    /// Carla's world object.
    cc::World world;
    /// Carla's debug helper object.
    cc::DebugHelper debug_helper;
    /// Pointers to messenger objects connecting stage pairs.
    std::shared_ptr<CollisionToPlannerMessenger> collision_planner_messenger;
    std::shared_ptr<LocalizationToCollisionMessenger> localization_collision_messenger;
    std::shared_ptr<LocalizationToTrafficLightMessenger> localization_traffic_light_messenger;
    std::shared_ptr<LocalizationToPlannerMessenger> localization_planner_messenger;
    std::shared_ptr<PlannerToControlMessenger> planner_control_messenger;
    std::shared_ptr<TrafficLightToPlannerMessenger> traffic_light_planner_messenger;
    /// Pointers to the stage objects of traffic manager.
    std::unique_ptr<CollisionStage> collision_stage;
    std::unique_ptr<BatchControlStage> control_stage;
    std::unique_ptr<LocalizationStage> localization_stage;
    std::unique_ptr<MotionPlannerStage> planner_stage;
    std::unique_ptr<TrafficLightStage> traffic_light_stage;
    /// Target velocity map for individual vehicles.
    AtomicMap<ActorId, float> vehicle_target_velocity;
    /// Map containing a set of actors to be ignored during collision detection.
    AtomicMap<ActorId, std::shared_ptr<AtomicActorSet>> ignore_collision;
    /// Map containing force lane change commands.
    AtomicMap<ActorId, bool> force_lane_change;
    /// Map containing auto lane change commands.
    AtomicMap<ActorId, bool> auto_lane_change;
    /// Static pointer to singleton object.
    static std::unique_ptr<TrafficManager> singleton_pointer;
    /// Singleton lifecycle management mutex.
    // static std::mutex singleton_mutex;
    /// Map containing distance to leading vehicle command.
    AtomicMap<ActorId, float> distance_to_leading_vehicle;

    /// Private constructor for singleton lifecycle management.
    TrafficManager(
        std::vector<float> longitudinal_PID_parameters,
        std::vector<float> longitudinal_highway_PID_parameters,
        std::vector<float> lateral_PID_parameters,
        float perc_decrease_from_limit,
        cc::Client &client_connection);

    /// To start the TrafficManager.
    void Start();

    /// To stop the TrafficManager.
    void Stop();

  public:

    /// Static method for singleton lifecycle management.
    static TrafficManager& GetInstance(cc::Client &client_connection);

    /// This method registers a vehicle with the traffic manager.
    void RegisterVehicles(const std::vector<ActorPtr> &actor_list);

    /// This method unregisters a vehicle from traffic manager.
    void UnregisterVehicles(const std::vector<ActorPtr> &actor_list);

    /// Set target velocity specific to a vehicle.
    void SetVehicleTargetVelocity(const ActorPtr &actor, const float velocity);

    /// Set collision detection rules between vehicles.
    void SetCollisionDetection(const ActorPtr &reference_actor,
                               const ActorPtr &other_actor,
                               const bool detect_collision);

    /// Method to force lane change on a vehicle.
    /// Direction flag can be set to true for left and false for right.
    void ForceLaneChange(const ActorPtr &actor, const bool direction);

    /// Enable / disable automatic lane change on a vehicle.
    void AutoLaneChange(const ActorPtr &actor, const bool enable);

    /// Method to specify how much distance a vehicle should maintain to
    /// the leading vehicle.
    void SetDistanceToLeadingVehicle(const ActorPtr &actor, const float distance);

    /// Destructor.
    ~TrafficManager();

  };

}
