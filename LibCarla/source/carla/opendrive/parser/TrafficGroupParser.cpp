// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "carla/opendrive/parser/TrafficGroupParser.h"

#include "carla/opendrive/pugixml/pugixml.hpp"

#include "carla/road/MapBuilder.h"

namespace carla {
namespace opendrive {
namespace parser {

  void TrafficGroupParser::Parse(
      const pugi::xml_document &/* xml */,
      carla::road::MapBuilder &/* map_builder */) {}

} // namespace parser
} // namespace opendrive
} // namespace carla
