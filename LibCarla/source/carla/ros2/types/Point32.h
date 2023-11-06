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
 * @file Point32.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifndef _FAST_DDS_GENERATED_GEOMETRY_MSGS_MSG_POINT32_H_
#define _FAST_DDS_GENERATED_GEOMETRY_MSGS_MSG_POINT32_H_

#include <fastrtps/utils/fixed_size_string.hpp>

#include <stdint.h>
#include <array>
#include <string>
#include <vector>
#include <map>
#include <bitset>

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#define eProsima_user_DllExport __declspec( dllexport )
#else
#define eProsima_user_DllExport
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define eProsima_user_DllExport
#endif  // _WIN32

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#if defined(Point32_SOURCE)
#define Point32_DllAPI __declspec( dllexport )
#else
#define Point32_DllAPI __declspec( dllimport )
#endif // Point32_SOURCE
#else
#define Point32_DllAPI
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define Point32_DllAPI
#endif // _WIN32

namespace eprosima {
namespace fastcdr {
class Cdr;
} // namespace fastcdr
} // namespace eprosima

namespace geometry_msgs {
    namespace msg {
        /*!
         * @brief This class represents the structure Point32 defined by the user in the IDL file.
         * @ingroup POINT32
         */
        class Point32
        {
        public:

            /*!
             * @brief Default constructor.
             */
            eProsima_user_DllExport Point32();

            /*!
             * @brief Default destructor.
             */
            eProsima_user_DllExport ~Point32();

            /*!
             * @brief Copy constructor.
             * @param x Reference to the object geometry_msgs::msg::Point32 that will be copied.
             */
            eProsima_user_DllExport Point32(
                    const Point32& x);

            /*!
             * @brief Move constructor.
             * @param x Reference to the object geometry_msgs::msg::Point32 that will be copied.
             */
            eProsima_user_DllExport Point32(
                    Point32&& x) noexcept;

            /*!
             * @brief Copy assignment.
             * @param x Reference to the object geometry_msgs::msg::Point32 that will be copied.
             */
            eProsima_user_DllExport Point32& operator =(
                    const Point32& x);

            /*!
             * @brief Move assignment.
             * @param x Reference to the object geometry_msgs::msg::Point32 that will be copied.
             */
            eProsima_user_DllExport Point32& operator =(
                    Point32&& x) noexcept;

            /*!
             * @brief Comparison operator.
             * @param x geometry_msgs::msg::Point32 object to compare.
             */
            eProsima_user_DllExport bool operator ==(
                    const Point32& x) const;

            /*!
             * @brief Comparison operator.
             * @param x geometry_msgs::msg::Point32 object to compare.
             */
            eProsima_user_DllExport bool operator !=(
                    const Point32& x) const;

            /*!
             * @brief This function sets a value in member x
             * @param _x New value for member x
             */
            eProsima_user_DllExport void x(
                    float _x);

            /*!
             * @brief This function returns the value of member x
             * @return Value of member x
             */
            eProsima_user_DllExport float x() const;

            /*!
             * @brief This function returns a reference to member x
             * @return Reference to member x
             */
            eProsima_user_DllExport float& x();

            /*!
             * @brief This function sets a value in member y
             * @param _y New value for member y
             */
            eProsima_user_DllExport void y(
                    float _y);

            /*!
             * @brief This function returns the value of member y
             * @return Value of member y
             */
            eProsima_user_DllExport float y() const;

            /*!
             * @brief This function returns a reference to member y
             * @return Reference to member y
             */
            eProsima_user_DllExport float& y();

            /*!
             * @brief This function sets a value in member z
             * @param _z New value for member z
             */
            eProsima_user_DllExport void z(
                    float _z);

            /*!
             * @brief This function returns the value of member z
             * @return Value of member z
             */
            eProsima_user_DllExport float z() const;

            /*!
             * @brief This function returns a reference to member z
             * @return Reference to member z
             */
            eProsima_user_DllExport float& z();

            /*!
            * @brief This function returns the maximum serialized size of an object
            * depending on the buffer alignment.
            * @param current_alignment Buffer alignment.
            * @return Maximum serialized size.
            */
            eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
                    size_t current_alignment = 0);

            /*!
             * @brief This function returns the serialized size of a data depending on the buffer alignment.
             * @param data Data which is calculated its serialized size.
             * @param current_alignment Buffer alignment.
             * @return Serialized size.
             */
            eProsima_user_DllExport static size_t getCdrSerializedSize(
                    const geometry_msgs::msg::Point32& data,
                    size_t current_alignment = 0);

            /*!
             * @brief This function serializes an object using CDR serialization.
             * @param cdr CDR serialization object.
             */
            eProsima_user_DllExport void serialize(
                    eprosima::fastcdr::Cdr& cdr) const;

            /*!
             * @brief This function deserializes an object using CDR serialization.
             * @param cdr CDR serialization object.
             */
            eProsima_user_DllExport void deserialize(
                    eprosima::fastcdr::Cdr& cdr);

            /*!
             * @brief This function returns the maximum serialized size of the Key of an object
             * depending on the buffer alignment.
             * @param current_alignment Buffer alignment.
             * @return Maximum serialized size.
             */
            eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
                    size_t current_alignment = 0);

            /*!
             * @brief This function tells you if the Key has been defined for this type
             */
            eProsima_user_DllExport static bool isKeyDefined();

            /*!
             * @brief This function serializes the key members of an object using CDR serialization.
             * @param cdr CDR serialization object.
             */
            eProsima_user_DllExport void serializeKey(
                    eprosima::fastcdr::Cdr& cdr) const;

        private:
            float m_x;
            float m_y;
            float m_z;
        };
    } // namespace msg
} // namespace geometry_msgs

#endif // _FAST_DDS_GENERATED_GEOMETRY_MSGS_MSG_POINT32_H_
