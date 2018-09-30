// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/Debug.h"
#include "carla/Memory.h"
#include "carla/sensor/DataMessage.h"
#include "carla/sensor/s11n/LidarMeasurement.h"

namespace carla {
namespace sensor {

  class SensorData;

namespace s11n {

  // ===========================================================================
  // -- LidarHeaderView --------------------------------------------------------
  // ===========================================================================

  class LidarHeaderView {
    using Index = LidarMeasurement::Index;
  public:

    float GetHorizontalAngle() const {
      return reinterpret_cast<const float &>(_begin[Index::HorizontalAngle]);
    }

    uint32_t GetChannelCount() const {
      return _begin[Index::ChannelCount];
    }

    uint32_t GetPointCount(size_t channel) const {
      DEBUG_ASSERT(channel < GetChannelCount());
      return _begin[Index::SIZE + channel];
    }

  private:

    friend class LidarSerializer;

    explicit LidarHeaderView(const uint32_t *begin) : _begin(begin) {
      DEBUG_ASSERT(_begin != nullptr);
    }

    const uint32_t *_begin;
  };

  // ===========================================================================
  // -- LidarSerializer --------------------------------------------------------
  // ===========================================================================

  class LidarSerializer {
  public:

    static LidarHeaderView DeserializeHeader(const DataMessage &message) {
      return LidarHeaderView{reinterpret_cast<const uint32_t *>(message.begin())};
    }

    static size_t GetHeaderOffset(const DataMessage &message) {
      auto View = DeserializeHeader(message);
      return sizeof(uint32_t) * (View.GetChannelCount() + LidarMeasurement::Index::SIZE);
    }

    template <typename Sensor>
    static Buffer Serialize(
        const Sensor &sensor,
        const LidarMeasurement &measurement,
        Buffer bitmap);

    static SharedPtr<SensorData> Deserialize(DataMessage message);
  };

  // ===========================================================================
  // -- LidarSerializer implementation -----------------------------------------
  // ===========================================================================

  template <typename Sensor>
  inline Buffer LidarSerializer::Serialize(
      const Sensor &,
      const LidarMeasurement &measurement,
      Buffer output) {
    std::array<boost::asio::const_buffer, 2u> seq = {
        boost::asio::buffer(measurement._header),
        boost::asio::buffer(measurement._points)};
    output.copy_from(seq);
    return output;
  }

} // namespace s11n
} // namespace sensor
} // namespace carla
