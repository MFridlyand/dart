/*
 * Copyright (c) 2011-2021, The DART development contributors
 * All rights reserved.
 *
 * The list of contributors can be found at:
 *   https://github.com/dartsim/dart/blob/master/LICENSE
 *
 * This file is provided under the following "BSD-style" License:
 *   Redistribution and use in source and binary forms, with or
 *   without modification, are permitted provided that the following
 *   conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *   USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *   POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "dart/collision/collision_result.hpp"
#include "dart/collision/fcl/backward_compatibility.hpp"
#include "dart/collision/fcl/fcl_engine.hpp"
#include "dart/collision/fcl/fcl_group.hpp"
#include "dart/collision/fcl/fcl_object.hpp"
#include "dart/collision/fcl/fcl_primitive_shape_utils.hpp"
#include "dart/common/logging.hpp"
#include "dart/math/geometry/sphere.hpp"

namespace dart {
namespace collision {

//==============================================================================
template <typename S>
Contact<S> convert_contact(
    const FclContact<S>& fcl_contact,
    FclObject<S>* object1,
    FclObject<S>* object2,
    const CollisionOption<S>& option)
{
  Contact<S> contact;

  contact.collision_object1 = object1;
  contact.collision_object2 = object2;

  if (option.enable_contact) {
    contact.point = to_vector3<S>(fcl_contact.pos);
    contact.normal = -to_vector3<S>(fcl_contact.normal);
    contact.depth = fcl_contact.penetration_depth;
  }

  return contact;
}

//==============================================================================
template <typename S>
void report_contacts(
    int num_contacts,
    const FclCollisionResult<S>& fcl_result,
    FclObject<S>* b1,
    FclObject<S>* b2,
    const CollisionOption<S>& option,
    CollisionResult<S>& result)
{
  for (auto i = 0; i < num_contacts; ++i) {
    result.add_contact(
        convert_contact(fcl_result.getContact(i), b1, b2, option));

    if (result.get_num_contacts() >= option.max_num_contacts) {
      return;
    }
  }
}

//==============================================================================
template <typename S>
std::shared_ptr<FclEngine<S>> FclEngine<S>::Create()
{
  return std::shared_ptr<FclEngine>(new FclEngine());
}

//==============================================================================
template <typename S>
FclEngine<S>::~FclEngine()
{
  // Do nothing
}

//==============================================================================
template <typename S>
const std::string& FclEngine<S>::get_type() const
{
  return GetType();
}

//==============================================================================
template <typename S>
const std::string& FclEngine<S>::GetType()
{
  static const std::string type = "fcl";
  return type;
}

//==============================================================================
template <typename S>
GroupPtr<S> FclEngine<S>::create_group()
{
  return std::make_shared<FclGroup<S>>(this);
}

//==============================================================================
template <typename S>
bool FclEngine<S>::collide(
    ObjectPtr<S> object1,
    ObjectPtr<S> object2,
    const CollisionOption<S>& option,
    CollisionResult<S>* result)
{
  auto derived1 = std::dynamic_pointer_cast<FclObject<S>>(object1);
  if (!derived1) {
    DART_ERROR("Invalid object");
    return false;
  }

  auto derived2 = std::dynamic_pointer_cast<FclObject<S>>(object2);
  if (!derived2) {
    DART_ERROR("Invalid object");
    return false;
  }

  auto fcl_collision_object1 = derived1->get_fcl_collision_object();
  if (!fcl_collision_object1) {
    DART_ERROR("Invalid object");
    return false;
  }

  auto fcl_collision_object2 = derived2->get_fcl_collision_object();
  if (!fcl_collision_object2) {
    DART_ERROR("Invalid object");
    return false;
  }

  FclCollisionRequest<S> fcl_request;
  fcl_request.enable_contact = option.enable_contact;
  fcl_request.num_max_contacts = option.max_num_contacts;

  FclCollisionResult<S> fcl_result;

  const auto num_contacts = ::fcl::collide(
      fcl_collision_object1, fcl_collision_object2, fcl_request, fcl_result);

  if (num_contacts > 0 && result) {
    report_contacts(
        num_contacts,
        fcl_result,
        derived1.get(),
        derived2.get(),
        option,
        *result);
  }

  return (num_contacts > 0);
}

//==============================================================================
template <typename S>
std::shared_ptr<FclCollisionGeometry<S>>
FclEngine<S>::create_fcl_collision_geometry(const math::ConstGeometryPtr& shape)
{
  FclCollisionGeometry<S>* geom = nullptr;
  const auto& shapeType = shape->get_type();

  if (auto sphere = shape->as<math::Sphere<S>>()) {
    const auto radius = sphere->get_radius();

    if (FclEngine<S>::PRIMITIVE == m_primitive_shape_type) {
      geom = new FclSphere<S>(radius);
    } else {
      auto fcl_mesh = new ::fcl::BVHModel<FclOBBRSS<S>>();
      auto fcl_sphere = FclSphere<S>(radius);
      ::fcl::generateBVHModel(
          *fcl_mesh, fcl_sphere, get_identity_transform<S>(), 16, 16);
      // TODO(JS): Consider using icosphere
      geom = fcl_mesh;
    }
  } else {
    DART_ERROR(
        "Attempting to create an unsupported shape type [{}]. Creating a "
        "sphere with 0.1 radius instead.",
        shapeType);
    geom = new FclSphere<S>(0.1);
  }

  assert(geom);

  return std::shared_ptr<FclCollisionGeometry<S>>(geom);
}

} // namespace collision
} // namespace dart