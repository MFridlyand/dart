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

#include "dart/collision/ode/ode_include.hpp"
#include "dart/collision/ode/ode_type.hpp"
#include "dart/collision/scene.hpp"
#include "dart/collision/type.hpp"

namespace dart {
namespace collision {

template <typename S_>
class OdeScene : public Scene<S_>
{
public:
  using S = S_;

  /// Constructor
  explicit OdeScene(Engine<S>* engine);

  /// Destructor
  ~OdeScene() override;

  // Documentation inherited
  ObjectPtr<S> create_object(math::GeometryPtr shape) override;

  // Documentation inherited
  bool collide(
      const CollisionOption<S>& option = {},
      CollisionResult<S>* result = nullptr) override
  {
    DART_NOT_IMPLEMENTED;
    DART_UNUSED(option, result);
    return false;
  };

protected:
  OdeEngine<S>* get_mutable_ode_engine();

  const OdeEngine<S>* get_ode_engine() const;

  dSpaceID get_ode_space_id() const;

private:
  friend class OdeEngine<S>;
  friend class OdeObject<S>;

  /// Top-level space for all sub-spaces/collisions
  dSpaceID m_ode_space_id;
};

DART_TEMPLATE_CLASS_HEADER(COLLISION, OdeScene)

} // namespace collision
} // namespace dart

#include "dart/collision/ode/detail/ode_scene_impl.hpp"
