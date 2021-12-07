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

#include <gtest/gtest.h>

#include "dart/math/all.hpp"

using namespace dart;
using namespace math;

//==============================================================================
template <typename T>
struct ProductLieGroupTest : public testing::Test
{
  using Type = T;
};

//==============================================================================
using Types = testing::Types<float /*, double, long double*/>;

//==============================================================================
TYPED_TEST_SUITE(ProductLieGroupTest, Types);

//==============================================================================
TYPED_TEST(ProductLieGroupTest, Basics)
{
  using Scalar = typename TestFixture::Type;

  auto a = ProductLieGroup<Scalar, SO3, SE3, R3>();
  EXPECT_EQ(
      a.SpaceDim,
      std::max({SO3<Scalar>::SpaceDim,
                SE3<Scalar>::SpaceDim,
                R3<Scalar>::SpaceDim}));
  EXPECT_EQ(
      a.GroupDim,
      SO3<Scalar>::GroupDim + SE3<Scalar>::GroupDim + R3<Scalar>::GroupDim);
  EXPECT_EQ(
      a.DataDim,
      SO3<Scalar>::DataDim + SE3<Scalar>::DataDim + R3<Scalar>::DataDim);
}
