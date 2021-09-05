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

#include "dart/math/geometry/convex3.hpp"
#include "dart/math/type.hpp"

namespace dart {
namespace math {

template <typename Scalar_>
class Plane3 : public Convex3<Scalar_>
{
public:
  // Type aliases
  using Scalar = Scalar_;
  using Vector3 = typename Convex3<Scalar>::Vector3;

  /// Returns type string
  static const std::string& GetType();

  /// Constructor
  explicit Plane3();

  // Documentation inherited
  const std::string& get_type() const override;

  /// Sets plane normal
  void set_normal(const math::Vector3<Scalar>& normal);

  /// Returns plane normal
  const math::Vector3<Scalar>& get_normal() const;

  /// Sets plane offset
  void set_offset(Scalar offset);

  /// Returns plane offset
  Scalar get_offset() const;

private:
  /// Plane normal
  math::Vector3<Scalar> m_normal;

  /// Plane offset
  Scalar m_offset;
};

using Plane3f = Plane3<float>;
using Plane3d = Plane3<double>;

extern template class DART_MATH_API Plane3<double>;

} // namespace math
} // namespace dart

#include "dart/math/geometry/detail/plane3_impl.hpp"
