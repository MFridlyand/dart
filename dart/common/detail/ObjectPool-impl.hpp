/*
 * Copyright (c) 2011-2022, The DART development contributors
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

#include <cstring>
#include <limits>
#include <type_traits>

#include "dart/common/Console.hpp"
#include "dart/common/Logging.hpp"
#include "dart/common/Macros.hpp"
#include "dart/common/Memory.hpp"
#include "dart/common/ObjectPool.hpp"

#define DART_OBJECT_POOL_DEFAULT_INITIAL_SIZE 4

namespace dart::common {

//==============================================================================
template <typename T>
ObjectPool<T>::ObjectPool(MemoryAllocator& allocator)
  : mMemoryAllocator(allocator)
{
  static_assert(
      sizeof(T) >= sizeof(void*),
      "The type size cannot be less than sizeof(void*).");

  //  if (!mFront)
  //  {
  //    DART_WARN("Not able to allocate base memory.");
  //    return;
  //  }

  //  T* it = mFront + DART_OBJECT_POOL_DEFAULT_INITIAL_SIZE - 1;
  //  for (; it != mFront; --it)
  //  {
  //    mFreeObjects.push(it);
  //  }
  //  mFreeObjects.push(it);

  createMemoryBlock(DART_OBJECT_POOL_DEFAULT_INITIAL_SIZE);
}

//==============================================================================
template <typename T>
ObjectPool<T>::~ObjectPool()
{
  // Lock the mutex
  std::lock_guard<std::mutex> lock(mMutex);

  if (!mFirstMemoryBlock)
  {
    DART_ASSERT(!mLastMemoryBlock);
    DART_ASSERT(!mFreeObjects.isEmpty());
    return;
  }

  auto* it = mFirstMemoryBlock;
  while (it)
  {
    auto* next = it->get_mutable_next_arena();
    mMemoryAllocator.destroy(it);
    it = next;
  }

  mFirstMemoryBlock = nullptr;
  mLastMemoryBlock = nullptr;
}

//==============================================================================
template <typename T>
const MemoryAllocator& ObjectPool<T>::getMemoryAllocator() const
{
  return mMemoryAllocator;
}

//==============================================================================
template <typename T>
MemoryAllocator& ObjectPool<T>::getMemoryAllocator()
{
  return mMemoryAllocator;
}

//==============================================================================
template <typename T>
template <typename... Args>
T* ObjectPool<T>::construct(Args&&... args) noexcept
{
  T* pointer = allocate();
  if (!pointer)
  {
    return nullptr;
  }

  // Call constructor. Return nullptr if failed.
  try
  {
    ::new (pointer) T(std::forward<Args>(args)...);
  }
  catch (...)
  {
    deallocate(pointer);
    return nullptr;
  }

  return reinterpret_cast<T*>(pointer);
}

//==============================================================================
template <typename T>
void ObjectPool<T>::destroy(T* object) noexcept
{
  if (!object)
  {
    return;
  }

  object->~T();
  deallocate(object);
}

//==============================================================================
template <typename T>
void ObjectPool<T>::print(std::ostream& os, int indent) const
{
  // Lock the mutex
  std::lock_guard<std::mutex> lock(mMutex);

  if (indent == 0)
  {
    os << "[ObjectPool]\n";
  }
  const std::string spaces(indent, ' ');
  if (indent != 0)
  {
    // os << spaces << "type: " << getType() << "\n";
  }
  os << spaces << "base_allocator:\n";
  mMemoryAllocator.print(os, indent + 2);
}

//==============================================================================
template <typename T>
T* ObjectPool<T>::allocate() noexcept
{
  // Lock the mutex
  std::lock_guard<std::mutex> lock(mMutex);

  if (mFreeObjects.isEmpty())
  {
    const bool success = createMemoryBlock(4);
    if (!success)
    {
      return nullptr;
    }
  }

  T* object = mFreeObjects.pop();

#ifndef NDEBUG // debug mode
  if (!object)
  {
    DART_DEBUG("Failed to allocate memory for {}.", typeid(T).name());
  }
#endif

  // DART_ASSERT(object);

  return object;
}

//==============================================================================
template <typename T>
void ObjectPool<T>::deallocate(T* pointer)
{
  // Cannot deallocate nullptr
  if (pointer == nullptr)
  {
    return;
  }

  // Lock the mutex
  std::lock_guard<std::mutex> lock(mMutex);
  mFreeObjects.push(std::move(pointer));
}

//==============================================================================
template <typename T>
bool ObjectPool<T>::createMemoryBlock(size_t requestedSize)
{
  DART_ASSERT(requestedSize > 0);

  detail::ObjectMemoryBlock<T>* newBlock
      = mMemoryAllocator.construct<detail::ObjectMemoryBlock<T>>(
          requestedSize, mMemoryAllocator);
  if (!newBlock)
  {
    return false;
  }

  if (!newBlock->is_valid())
  {
    mMemoryAllocator.destroy(newBlock);
    return false;
  }

  if (!mFirstMemoryBlock)
  {
    mFirstMemoryBlock = newBlock;
  }
  else if (mLastMemoryBlock)
  {
    mLastMemoryBlock->set_next_arena(newBlock);
  }
  mLastMemoryBlock = newBlock;

  const T* firstObjct = newBlock->get_first_item();
  T* it = newBlock->get_last_item();
  for (; it != firstObjct; --it)
  {
    mFreeObjects.push(it);
  }
  mFreeObjects.push(it);

  return true;
}

} // namespace dart::common
