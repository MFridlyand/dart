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

#include "dart/common/all.hpp"

using namespace dart;
using namespace common;

//==============================================================================
TEST(ObjectPoolTest, Basics)
{
  auto alloc = ObjectPool<double>(2);
  EXPECT_EQ(alloc.size(), 0);

  auto o1 = alloc.construct();
  EXPECT_EQ(alloc.size(), 1);
  EXPECT_EQ(o1, alloc.get_front());

  auto o2 = alloc.construct();
  EXPECT_EQ(alloc.size(), 2);
  EXPECT_EQ(o2, alloc.get_front() + 1);

  auto o3 = alloc.construct();
  EXPECT_EQ(alloc.size(), 2);
  EXPECT_EQ(o3, nullptr);

  alloc.destroy(o1);
  alloc.destroy(o2);

#ifndef NDEBUG
  alloc.print_free_object_stack();
#endif
}

//==============================================================================
// TEST(ObjectPoolTest, DynamicObjectPool)
//{
//  auto alloc = DynamicObjectPool<double>(2);
//}
