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

#include <vector>

#include <gtest/gtest.h>

#include "dart/common/linear_allocator.hpp"
#include "dart/common/logging.hpp"

using namespace dart::common;

//==============================================================================
TEST(LinearAllocatorTest, ConstructorsAndInitialStates)
{
#ifndef NDEBUG
  set_log_level(LogLevel::LL_DEBUG);
#endif

  auto alloc1 = LinearAllocator(0);
  EXPECT_EQ(alloc1.get_max_capacity(), 0);
  EXPECT_EQ(alloc1.get_size(), 0);
  EXPECT_EQ(alloc1.get_begin_address(), nullptr);

  auto alloc2 = LinearAllocator(64);
  EXPECT_EQ(alloc2.get_max_capacity(), 64);
  EXPECT_EQ(alloc2.get_size(), 0);
  EXPECT_NE(alloc2.get_begin_address(), nullptr);
}

//==============================================================================
TEST(AllocatorTest, TotalSize)
{
#ifndef NDEBUG
  set_log_level(LogLevel::LL_DEBUG);
#endif

  auto allocator1 = LinearAllocator(0);
  EXPECT_TRUE(allocator1.allocate(0) == nullptr);
  EXPECT_TRUE(allocator1.allocate(1) == nullptr);
  EXPECT_EQ(allocator1.get_size(), 0);

  auto allocator2 = LinearAllocator(4);

  EXPECT_TRUE(allocator2.allocate(0) == nullptr);
  EXPECT_EQ(allocator2.get_size(), 0);

  EXPECT_TRUE(allocator2.allocate(1) != nullptr);
  EXPECT_EQ(allocator2.get_size(), 1);

  EXPECT_TRUE(allocator2.allocate(2) != nullptr);
  EXPECT_EQ(allocator2.get_size(), 3);

  EXPECT_TRUE(allocator2.allocate(3) == nullptr);
  EXPECT_EQ(allocator2.get_size(), 3);

  EXPECT_TRUE(allocator2.allocate(1) != nullptr);
  EXPECT_EQ(allocator2.get_size(), 4);

  EXPECT_TRUE(allocator2.allocate(1) == nullptr);
  EXPECT_EQ(allocator2.get_size(), 4);

  struct DoubleInt
  {
    int a;
    int b;
  };

  static_assert(sizeof(DoubleInt) > 1);
  auto allocator3 = LinearAllocator(sizeof(DoubleInt) + 1);

  EXPECT_TRUE(allocator3.construct<DoubleInt>() != nullptr);
  EXPECT_EQ(allocator3.get_size(), sizeof(DoubleInt));
  EXPECT_TRUE(allocator3.construct<DoubleInt>() == nullptr);
  EXPECT_EQ(allocator3.get_size(), sizeof(DoubleInt));

  allocator3.destroy<DoubleInt>(nullptr);
}
