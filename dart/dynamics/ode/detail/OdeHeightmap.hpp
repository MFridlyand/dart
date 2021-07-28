/*
 * Copyright (c) 2011-2021, The DART development contributors
 * All rights reserved.
 *
 * The list of contributors can be found at:
 *   https://github.com/dartsim/dart/blob/main/LICENSE
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

#ifndef DART_COLLISION_ODE_DETAIL_ODEHEIGHTMAP_HPP_
#define DART_COLLISION_ODE_DETAIL_ODEHEIGHTMAP_HPP_

#include <ode/ode.h>

#include "dart/dynamics/HeightmapShape.hpp"
#include "dart/dynamics/ode/detail/OdeGeom.hpp"

namespace dart {
namespace dynamics {
namespace detail {

template <typename S>
class OdeHeightmap : public OdeGeom
{
public:
  /// Constructor
  OdeHeightmap(
      const OdeCollisionObject* parent, const dynamics::HeightmapShape<S>* hs);

  /// Destructor
  ~OdeHeightmap() override;

private:
  dHeightfieldDataID mOdeHeightfieldId;
};

using OdeHeightmapf = OdeHeightmap<float>;
using OdeHeightmapd = OdeHeightmap<double>;

} // namespace detail
} // namespace dynamics
} // namespace dart

#include "dart/dynamics/ode/detail/OdeHeightmap-impl.hpp"

#endif // DART_COLLISION_ODE_DETAIL_ODEHEIGHTMAP_HPP_
