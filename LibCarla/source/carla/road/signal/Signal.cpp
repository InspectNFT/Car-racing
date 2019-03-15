// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy; see <https://opensource.org/licenses/MIT>.

#include "Signal.h"

void carla::road::signal::Signal::AddValidity(carla::road::general::Validity &&validity) {
  _validities.push_back(std::move(validity));
}

void carla::road::signal::Signal::AddDependency(carla::road::signal::SignalDependency &&dependency) {
  _dependencies.push_back(std::move(dependency));
}