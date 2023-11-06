// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * @file CarlaCollisionEvent.cpp
 * This source file contains the definition of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifdef _WIN32
// Remove linker warning LNK4221 on Visual Studio
namespace {
char dummy;
}  // namespace
#endif  // _WIN32

#include "CarlaCollisionEvent.h"
#include <fastcdr/Cdr.h>

#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>

#define carla_msgs_msg_geometry_msgs_msg_Vector3_max_cdr_typesize 24ULL;
#define carla_msgs_msg_std_msgs_msg_Header_max_cdr_typesize 268ULL;
#define carla_msgs_msg_CollisionEvent_max_cdr_typesize 296ULL;
#define carla_msgs_msg_std_msgs_msg_Time_max_cdr_typesize 8ULL;
#define carla_msgs_msg_geometry_msgs_msg_Vector3_max_key_cdr_typesize 0ULL;
#define carla_msgs_msg_std_msgs_msg_Header_max_key_cdr_typesize 0ULL;
#define carla_msgs_msg_CollisionEvent_max_key_cdr_typesize 0ULL;
#define carla_msgs_msg_std_msgs_msg_Time_max_key_cdr_typesize 0ULL;

carla_msgs::msg::CollisionEvent::CollisionEvent()
{
    // std_msgs::msg::Header m_header
    // unsigned long m_other_actor_id
    m_other_actor_id = 0;
    // geometry_msgs::msg::Vector3 m_normal_impulse
}

carla_msgs::msg::CollisionEvent::~CollisionEvent()
{
}

carla_msgs::msg::CollisionEvent::CollisionEvent(
        const CollisionEvent& x)
{
    m_header = x.m_header;
    m_other_actor_id = x.m_other_actor_id;
    m_normal_impulse = x.m_normal_impulse;
}

carla_msgs::msg::CollisionEvent::CollisionEvent(
        CollisionEvent&& x) noexcept
{
    m_header = std::move(x.m_header);
    m_other_actor_id = x.m_other_actor_id;
    m_normal_impulse = std::move(x.m_normal_impulse);
}

carla_msgs::msg::CollisionEvent& carla_msgs::msg::CollisionEvent::operator =(
        const CollisionEvent& x)
{
    m_header = x.m_header;
    m_other_actor_id = x.m_other_actor_id;
    m_normal_impulse = x.m_normal_impulse;

    return *this;
}

carla_msgs::msg::CollisionEvent& carla_msgs::msg::CollisionEvent::operator =(
        CollisionEvent&& x) noexcept
{
    m_header = std::move(x.m_header);
    m_other_actor_id = x.m_other_actor_id;
    m_normal_impulse = std::move(x.m_normal_impulse);

    return *this;
}

bool carla_msgs::msg::CollisionEvent::operator ==(
        const CollisionEvent& x) const
{
    return (m_header == x.m_header && m_other_actor_id == x.m_other_actor_id && m_normal_impulse == x.m_normal_impulse);
}

bool carla_msgs::msg::CollisionEvent::operator !=(
        const CollisionEvent& x) const
{
    return !(*this == x);
}

size_t carla_msgs::msg::CollisionEvent::getMaxCdrSerializedSize(
        size_t current_alignment)
{
    static_cast<void>(current_alignment);
    return carla_msgs_msg_CollisionEvent_max_cdr_typesize;
}

size_t carla_msgs::msg::CollisionEvent::getCdrSerializedSize(
        const carla_msgs::msg::CollisionEvent& data,
        size_t current_alignment)
{
    (void)data;
    size_t initial_alignment = current_alignment;
    current_alignment += std_msgs::msg::Header::getCdrSerializedSize(data.header(), current_alignment);
    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);
    current_alignment += geometry_msgs::msg::Vector3::getCdrSerializedSize(data.normal_impulse(), current_alignment);
    return current_alignment - initial_alignment;
}

void carla_msgs::msg::CollisionEvent::serialize(
        eprosima::fastcdr::Cdr& scdr) const
{
    scdr << m_header;
    scdr << m_other_actor_id;
    scdr << m_normal_impulse;
}

void carla_msgs::msg::CollisionEvent::deserialize(
        eprosima::fastcdr::Cdr& dcdr)
{
    dcdr >> m_header;
    dcdr >> m_other_actor_id;
    dcdr >> m_normal_impulse;
}

/*!
 * @brief This function copies the value in member header
 * @param _header New value to be copied in member header
 */
void carla_msgs::msg::CollisionEvent::header(
        const std_msgs::msg::Header& _header)
{
    m_header = _header;
}

/*!
 * @brief This function moves the value in member header
 * @param _header New value to be moved in member header
 */
void carla_msgs::msg::CollisionEvent::header(
        std_msgs::msg::Header&& _header)
{
    m_header = std::move(_header);
}

/*!
 * @brief This function returns a constant reference to member header
 * @return Constant reference to member header
 */
const std_msgs::msg::Header& carla_msgs::msg::CollisionEvent::header() const
{
    return m_header;
}

/*!
 * @brief This function returns a reference to member header
 * @return Reference to member header
 */
std_msgs::msg::Header& carla_msgs::msg::CollisionEvent::header()
{
    return m_header;
}

/*!
 * @brief This function sets a value in member other_actor_id
 * @param _other_actor_id New value for member other_actor_id
 */
void carla_msgs::msg::CollisionEvent::other_actor_id(
        uint32_t _other_actor_id)
{
    m_other_actor_id = _other_actor_id;
}

/*!
 * @brief This function returns the value of member other_actor_id
 * @return Value of member other_actor_id
 */
uint32_t carla_msgs::msg::CollisionEvent::other_actor_id() const
{
    return m_other_actor_id;
}

/*!
 * @brief This function returns a reference to member other_actor_id
 * @return Reference to member other_actor_id
 */
uint32_t& carla_msgs::msg::CollisionEvent::other_actor_id()
{
    return m_other_actor_id;
}

/*!
 * @brief This function copies the value in member normal_impulse
 * @param _normal_impulse New value to be copied in member normal_impulse
 */
void carla_msgs::msg::CollisionEvent::normal_impulse(
        const geometry_msgs::msg::Vector3& _normal_impulse)
{
    m_normal_impulse = _normal_impulse;
}

/*!
 * @brief This function moves the value in member normal_impulse
 * @param _normal_impulse New value to be moved in member normal_impulse
 */
void carla_msgs::msg::CollisionEvent::normal_impulse(
        geometry_msgs::msg::Vector3&& _normal_impulse)
{
    m_normal_impulse = std::move(_normal_impulse);
}

/*!
 * @brief This function returns a constant reference to member normal_impulse
 * @return Constant reference to member normal_impulse
 */
const geometry_msgs::msg::Vector3& carla_msgs::msg::CollisionEvent::normal_impulse() const
{
    return m_normal_impulse;
}

/*!
 * @brief This function returns a reference to member normal_impulse
 * @return Reference to member normal_impulse
 */
geometry_msgs::msg::Vector3& carla_msgs::msg::CollisionEvent::normal_impulse()
{
    return m_normal_impulse;
}

size_t carla_msgs::msg::CollisionEvent::getKeyMaxCdrSerializedSize(
        size_t current_alignment)
{
    static_cast<void>(current_alignment);
    return carla_msgs_msg_CollisionEvent_max_key_cdr_typesize;
}

bool carla_msgs::msg::CollisionEvent::isKeyDefined()
{
    return false;
}

void carla_msgs::msg::CollisionEvent::serializeKey(
        eprosima::fastcdr::Cdr& scdr) const
{
    (void) scdr;
}
