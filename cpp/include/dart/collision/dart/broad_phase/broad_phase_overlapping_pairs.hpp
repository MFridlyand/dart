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

#include <unordered_map>

#include "dart/collision/dart/broad_phase/nearby_object_pair.hpp"
#include "dart/collision/dart/dart_type.hpp"
#include "dart/collision/dart/narrow_phase/collision_algorithm_manager.hpp"
#include "dart/common/hash.hpp"

namespace dart::collision::detail {

/// Container specialized for storing collision object pairs that are usually
/// detected by the broad phase collision detection
template <typename Scalar_>
class BroadPhaseOverlappingPairs
{
public:
  // Type aliases
  using Scalar = Scalar_;

  /// Constructor
  BroadPhaseOverlappingPairs(
      CollisionAlgorithmSelector<Scalar>& collision_algorithm_manager)
    : m_collision_algorithm_manager(collision_algorithm_manager)
  {
    // Do nothing
  }

  /// Destructor
  virtual ~BroadPhaseOverlappingPairs() = default;

  bool add(DartObject<Scalar>* object_a, DartObject<Scalar>* object_b);

  // bool remove(DartObject<Scalar>* object_a, DartObject<Scalar>* object_b);

  void request_narrow_phase()
  {
    for (auto i = 0u; i < m_pair_id_list.size(); ++i) {
      auto& algorithm = m_algorithms[i];
      algorithm->request_collision_check(m_objects_a[i], m_objects_b[i]);
    }
  }

private:
  struct OverlappingObjectPairHash
  {
    std::size_t operator()(const std::pair<ObjectId, ObjectId>& key) const
    {
      return common::hash_pair_szudzik_ascend(key.first, key.second);
    }
  };

  detail::CollisionAlgorithmSelector<Scalar>& m_collision_algorithm_manager;

  // TODO(JS): Use custom memory allocator (+ custom container if needed)
  std::vector<ObjectPairId> m_pair_id_list;
  std::vector<DartObject<Scalar>*> m_objects_a;
  std::vector<DartObject<Scalar>*> m_objects_b;
  std::vector<CollisionAlgorithm<Scalar>*> m_algorithms; // TODO(JS): Remove?
  std::vector<bool> m_should_test_overlap;

  std::unordered_map<ObjectPairId, uint64_t> m_map_pair_id_to_index;

  uint64_t m_pair_count = 0;
};

} // namespace dart::collision::detail

#include "dart/collision/dart/broad_phase/broad_phase_overlapping_pairs_impl.hpp"
