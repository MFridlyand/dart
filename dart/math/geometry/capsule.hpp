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

#include "dart/math/export.hpp"
#include "dart/math/geometry/convex3.hpp"
#include "dart/math/type.hpp"

namespace dart {
namespace math {

template <typename S_>
class Capsule : public Convex3<S_> {
public:
  // Type aliases
  using S = S_;

  /// Returns type string
  static const std::string& GetType();

  /// Constructor
  ///
  /// \param[in] radius: Radius of the capsule
  /// \param[in] height: Height of the cylindrical part excluding the dome parts
  Capsule(S radius, S height);

  // Documentation inherited
  const std::string& get_type() const override;

  S get_radius() const
  {
    return m_radius;
  }
  S get_height() const
  {
    return m_height;
  }

private:
  S m_radius;
  S m_height;
};

using Capsulef = Capsule<float>;
using Capsuled = Capsule<double>;

extern template class DART_MATH_API Capsule<double>;

} // namespace math
} // namespace dart

#include "dart/math/geometry/detail/capsule_impl.hpp"
