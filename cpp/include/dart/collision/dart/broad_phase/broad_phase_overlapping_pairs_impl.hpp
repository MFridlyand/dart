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

#include "dart/collision/dart/broad_phase/broad_phase_overlapping_pairs.hpp"
#include "dart/collision/dart/dart_object.hpp"

namespace dart::collision::detail {

//==============================================================================
template <typename Scalar>
bool BroadPhaseOverlappingPairs<Scalar>::add(
    DartObject<Scalar>* object_a, DartObject<Scalar>* object_b)
{
  // Not allowed to add the same objects as a pair
  if (object_a == object_b) {
    DART_DEBUG("Cannot add the same collision objects as a pair.");
    return false;
  }

  // Sort the objects by the IDs
  if (object_a->get_id() > object_b->get_id()) {
    std::swap(object_a, object_b);
  }

  const ObjectId id_a = object_a->get_id();
  const ObjectId id_b = object_b->get_id();

  // IDs should be sorted.
  DART_ASSERT(id_a < id_b);

  // Two distinct collision objects shouldn't have the same ID.
  DART_ASSERT(id_a != id_b);

  const ObjectPairId pair_id = common::hash_pair_szudzik_ascend(id_a, id_b);
  if (m_map_pair_id_to_index.find(pair_id) != m_map_pair_id_to_index.end()) {
    DART_DEBUG("Cannot add the same pair twice.");
    return false;
  }

  const uint64_t index = m_pair_count++;
  auto collision_algorithm
      = m_collision_algorithm_manager.create_algorithm(object_a, object_b);

  m_pair_id_list.push_back(pair_id);
  m_objects_a.push_back(object_a);
  m_objects_b.push_back(object_b);
  m_algorithms.push_back(collision_algorithm);

  m_map_pair_id_to_index[pair_id] = index;

  return true;
}

//==============================================================================
// template <typename Scalar>
// bool BroadPhaseOverlappingPairs<Scalar>::remove(
//    DartObject<Scalar>* object_a, DartObject<Scalar>* object_b)
//{
//  DART_ASSERT(object_a);
//  DART_ASSERT(object_b);
//  DART_ASSERT(object_a != object_b);

//  // Sort the objects by the IDs
//  if (object_a->get_id() > object_b->get_id()) {
//    std::swap(object_a, object_b);
//  }

//  const ObjectId id_a = object_a->get_id();
//  const ObjectId id_b = object_b->get_id();

//  // IDs should be sorted.
//  DART_ASSERT(id_a < id_b);

//  // Two distinct collision objects shouldn't have the same ID.
//  DART_ASSERT(id_a != id_b);

//  const auto it = m_pairs.find({id_a, id_b});
//  if (it == m_pairs.end()) {
//    DART_DEBUG("Cannot add the same pair twice.");
//    return false;
//  }

//  // Release resources
//  OverlappingObjectPair<Scalar>& nearby_object_pair = it->second;
//  if (nearby_object_pair.algorithm != nullptr) {
//    // TODO(JS): Deallocate collision algorithm
//    DART_NOT_IMPLEMENTED;
//  }

//  m_pairs.erase(it);

//  return true;
//}

} // namespace dart::collision::detail
