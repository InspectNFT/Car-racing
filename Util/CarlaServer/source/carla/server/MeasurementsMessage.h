// CARLA, Copyright (C) 2017 Computer Vision Center (CVC) Project Settings.

#pragma once

#include "carla/NonCopyable.h"
#include "carla/server/ImagesMessage.h"

namespace carla {
namespace server {

  class MeasurementsMessage : private NonCopyable {
  public:

    void Write(
        const carla_measurements &measurements,
        const_array_view<carla_image> images) {
      _measurements = measurements;
      _images.Write(images);
    }

    const carla_measurements &measurements() const {
      return _measurements;
    }

    const_buffer images() const {
      return _images.buffer();
    }

  private:

    carla_measurements _measurements;

    ImagesMessage _images;
  };

} // namespace server
} // namespace carla
