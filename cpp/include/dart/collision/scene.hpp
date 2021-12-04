/*
 * Copyright (c) 2011-2021, The DART development contributors:
 * https://github.com/dartsim/dart/blob/main/LICENSE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <memory>

#include "dart/collision/export.hpp"
#include "dart/collision/type.hpp"
#include "dart/common/container/vector.hpp"
#include "dart/math/geometry/type.hpp"

namespace dart::collision {

template <typename Scalar_>
class Scene
{
public:
  // Type aliases
  using Scalar = Scalar_;

  /// Destructor
  virtual ~Scene();

  /// Return collision detection engine associated with this Scene
  Engine<Scalar>* get_mutable_engine();

  /// Return (const) collision detection engine associated with this
  /// Scene
  const Engine<Scalar>* get_engine() const;

  /// Creates a collision object.
  Object<Scalar>* create_object(math::Geometry3Ptr<Scalar> geometry);

  void destroy_object(Object<Scalar>* object);

  int get_object_count() const;

  ConstObjectPtr<Scalar> get_object_by_index(int index) const;

  /// Creates a collision object with sphere
  template <typename... Args>
  Object<Scalar>* create_sphere_object(Args&&... args);

  virtual void update(Scalar time_step = 1e-3) = 0;

protected:
  /// Constructor
  ///
  /// \param[in] collisionDetector: Collision detector that created this group.
  Scene(Engine<Scalar>* engine);

  virtual Object<Scalar>* create_object_impl(
      ObjectId id, math::Geometry3Ptr<Scalar> geometry)
      = 0;

  virtual void destroy_object_impl(Object<Scalar>* object) = 0;

  virtual const ObjectArray<Scalar>& get_objects() const = 0;

  virtual ObjectArray<Scalar>& get_mutable_objects() = 0;

  /// The parent collision engine that created this scene
  Engine<Scalar>* m_engine;

  std::vector<ObjectPtr<Scalar>> m_objects;

private:
  ObjectId m_next_collision_object_id = 0;
};

DART_TEMPLATE_CLASS_HEADER(COLLISION, Scene)

} // namespace dart::collision

#include "dart/collision/detail/scene_impl.hpp"
