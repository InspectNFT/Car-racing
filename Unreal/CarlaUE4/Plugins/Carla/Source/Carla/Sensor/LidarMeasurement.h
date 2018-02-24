// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "Sensor/SensorDataView.h"

#include "Containers/Array.h"

/// Stores the data generated by ALidar. To be used by ALidar solely.
///
/// The header consists of an array of uint32's in the following layout
///
///    {
///      Horizontal angle (float),
///      Channel count,
///      Point count of channel 0,
///      ...
///      Point count of channel n,
///    }
///
/// The points are stored in an array of floats
///
///    {
///      X0, Y0, Z0,
///      ...
///      Xn, Yn, Zn,
///    }
///
class FLidarMeasurement {
  static_assert(sizeof(float) == sizeof(uint32), "Invalid float size");
public:

  explicit FLidarMeasurement(uint32 SensorId = 0u, uint32 ChannelCount = 0u)
    : SensorId(SensorId)
  {
    Header.AddDefaulted(2u + ChannelCount);
    Header[1] = ChannelCount;
  }

  FLidarMeasurement &operator=(FLidarMeasurement &&Other)
  {
    SensorId = Other.SensorId;
    Header = std::move(Other.Header);
    Points = std::move(Other.Points);
    Other.SensorId = 0u;
    return *this;
  }

  float GetHorizontalAngle() const
  {
    return reinterpret_cast<const float &>(Header[0]);
  }

  void SetHorizontalAngle(float HorizontalAngle)
  {
    Header[0] = reinterpret_cast<const uint32 &>(HorizontalAngle);
  }

  uint32 GetChannelCount() const
  {
    return Header[1];
  }

  void Reset(uint32 TotalPointCount)
  {
    std::memset(Header.GetData() + 2u, 0, sizeof(uint32) * GetChannelCount());
    Points.Reset(3u * TotalPointCount);
  }

  void WritePoint(uint32 Channel, const FVector &Point)
  {
    check(Header[1] > Channel);
    Header[2u + Channel] += 1u;
    Points.Emplace(Point.X);
    Points.Emplace(Point.Y);
    Points.Emplace(Point.Z);
  }

  FSensorDataView GetView() const
  {
    return FSensorDataView(SensorId, Header, Points);
  }

private:

  uint32 SensorId;

  TArray<uint32> Header;

  TArray<float> Points;
};
