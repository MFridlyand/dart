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

#include "dart/common/hash.hpp"
#include "dart/common/macro.hpp"

namespace dart::common {

//==============================================================================
template <typename T>
T hash_pair_cantor_pairing(T a, T b)
{
  static_assert(std::is_unsigned_v<T>, "Cannot use for signed integers.");
  return 0.5 * (a + b) * (a + b + 1) + b;
}

//==============================================================================
template <typename T>
T hash_pair_szudzik(T a, T b)
{
  static_assert(std::is_unsigned_v<T>, "Cannot use for signed integers.");
  return a >= b ? a * a + a + b : a + b * b;
}

//==============================================================================
template <typename T>
T hash_pair_szudzik_ascend(T a, T b)
{
  static_assert(std::is_unsigned_v<T>, "Cannot use for signed integers.");
  DART_ASSERT(a <= b);
  return a + b * b;
}

//==============================================================================
template <typename T>
T hash_pair_szudzik_descend(T a, T b)
{
  static_assert(std::is_unsigned_v<T>, "Cannot use for signed integers.");
  DART_ASSERT(b <= a);
  return a * a + a + b;
}

} // namespace dart::common
