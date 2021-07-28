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

#include <Eigen/Core>

#include "dart/math/geometry/convex3.hpp"

namespace dart {
namespace math {

template <typename S_>
class Cuboid : public Convex3<S_>
{
public:
  // Type aliases
  using S = S_;
  using Vector3 = typename Convex3<S>::Vector3;

  /// Returns type string
  static const std::string& GetType();

  /// Computes volume given size
  static S ComputeVolume(const Vector3& size);

  /// Constructor
  ///
  /// \param[in] length: The length of each side of the Cuboid.
  explicit Cuboid(const Vector3& size = Vector3::Ones());

  // Documentation inherited
  const std::string& get_type() const override;

  /// Returns the size
  const Vector3& get_size() const;

  /// Sets the size
  void set_size(const Vector3& length);

private:
  /// The lengths of the cuboid
  Vector3 m_size;
};

using Cuboidf = Cuboid<float>;
using Cuboidd = Cuboid<double>;

extern template class DART_MATH_API Cuboid<double>;

} // namespace math
} // namespace dart

#include "dart/math/geometry/detail/cuboid_impl.hpp"
