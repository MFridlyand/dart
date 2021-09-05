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

#include "dart/math/Geometry.hpp"
#include "dart/math/geometry/tri_mesh.hpp"

namespace dart {
namespace math {

//==============================================================================
template <typename Scalar>
TriMesh<Scalar>::TriMesh()
{
  // Do nothing
}

//==============================================================================
template <typename Scalar>
const std::string& TriMesh<Scalar>::GetType()
{
  static const std::string type("TriMesh");
  return type;
}

//==============================================================================
template <typename Scalar>
const std::string& TriMesh<Scalar>::get_type() const
{
  return GetType();
}

//==============================================================================
template <typename Scalar>
void TriMesh<Scalar>::reserve_triangles(int size)
{
  m_triangles.reserve(size);
}

//==============================================================================
template <typename Scalar>
void TriMesh<Scalar>::set_triangles(
    const Vertices& vertices, const Triangles& triangles)
{
  clear();

  this->m_vertices = vertices;
  m_triangles = triangles;
}

//==============================================================================
template <typename Scalar>
void TriMesh<Scalar>::add_triangle(const Triangle& triangle)
{
  m_triangles.push_back(triangle);
}

//==============================================================================
template <typename Scalar>
int TriMesh<Scalar>::get_num_triangles() const
{
  return m_triangles.size();
}

//==============================================================================
template <typename Scalar>
void TriMesh<Scalar>::compute_vertex_normals()
{
  compute_triangle_normals();

  this->m_vertex_normals.clear();
  this->m_vertex_normals.resize(this->m_vertices.size(), Vector3::Zero());

  for (auto i = 0u; i < m_triangles.size(); ++i) {
    auto& triangle = m_triangles[i];
    this->m_vertex_normals[triangle[0]] += m_triangle_normals[i];
    this->m_vertex_normals[triangle[1]] += m_triangle_normals[i];
    this->m_vertex_normals[triangle[2]] += m_triangle_normals[i];
  }

  this->normalize_vertex_vormals();
}

//==============================================================================
template <typename Scalar>
bool TriMesh<Scalar>::has_triangles() const
{
  return !m_triangles.empty();
}

//==============================================================================
template <typename Scalar>
bool TriMesh<Scalar>::has_triangle_normals() const
{
  return has_triangles() && m_triangles.size() == m_triangle_normals.size();
}

//==============================================================================
template <typename Scalar>
const typename TriMesh<Scalar>::Triangles& TriMesh<Scalar>::get_triangles()
    const
{
  return m_triangles;
}

//==============================================================================
template <typename Scalar>
const typename TriMesh<Scalar>::Normals& TriMesh<Scalar>::get_triangle_normals()
    const
{
  return m_triangle_normals;
}

//==============================================================================
template <typename Scalar>
void TriMesh<Scalar>::clear()
{
  m_triangles.clear();
  m_triangle_normals.clear();
  Base::clear();
}

//==============================================================================
template <typename Scalar>
TriMesh<Scalar> TriMesh<Scalar>::operator+(const TriMesh& other) const
{
  return (TriMesh(*this) += other);
}

//==============================================================================
template <typename Scalar>
TriMesh<Scalar>& TriMesh<Scalar>::operator+=(const TriMesh& other)
{
  if (other.is_empty())
    return *this;

  const auto oldNumVertices = this->m_vertices.size();
  const auto oldNumTriangles = m_triangles.size();

  Base::operator+=(other);

  // Insert triangle normals if both meshes have normals. Otherwise, clean the
  // triangle normals.
  if ((!has_triangles() || has_triangle_normals())
      && other.has_triangle_normals()) {
    m_triangle_normals.insert(
        m_triangle_normals.end(),
        other.m_triangle_normals.begin(),
        other.m_triangle_normals.end());
  } else {
    m_triangle_normals.clear();
  }

  const Triangle offset = Triangle::Constant(oldNumVertices);
  m_triangles.resize(m_triangles.size() + other.m_triangles.size());
  for (auto i = 0u; i < other.m_triangles.size(); ++i) {
    m_triangles[i + oldNumTriangles] = other.m_triangles[i] + offset;
  }

  return *this;
}

//==============================================================================
template <typename Scalar>
Scalar TriMesh<Scalar>::get_volume() const
{
  // Reference: Zhang and Chen, "Efficient feature extraction for 2D/3D objects
  // in mesh representation," 2001

  auto compute_volume = [&](const Triangle& triangle) {
    const Vector3& v0 = this->m_vertices[triangle[0]];
    const Vector3& v1 = this->m_vertices[triangle[1]];
    const Vector3& v2 = this->m_vertices[triangle[2]];
    return v0.dot(v1.cross(v2)) / 6.0;
  };

  Scalar volume = 0;

  for (const auto& triangle : m_triangles) {
    volume += compute_volume(triangle);
  }

  return std::abs(volume);
}

//==============================================================================
template <typename Scalar>
std::shared_ptr<TriMesh<Scalar>> TriMesh<Scalar>::generate_convex_hull(
    bool optimize) const
{
  auto triangles = Triangles();
  auto vertices = Vertices();
  std::tie(vertices, triangles)
      = compute_convex_hull_3d<Scalar, Index>(this->m_vertices, optimize);

  auto mesh = std::make_shared<TriMesh<Scalar>>();
  mesh->set_triangles(vertices, triangles);

  return mesh;
}

//==============================================================================
template <typename Scalar>
void TriMesh<Scalar>::compute_triangle_normals()
{
  m_triangle_normals.resize(m_triangles.size());

  for (auto i = 0u; i < m_triangles.size(); ++i) {
    auto& triangle = m_triangles[i];
    const Vector3 v01
        = this->m_vertices[triangle[1]] - this->m_vertices[triangle[0]];
    const Vector3 v02
        = this->m_vertices[triangle[2]] - this->m_vertices[triangle[0]];
    m_triangle_normals[i] = v01.cross(v02);
  }

  normalize_triangle_normals();
}

//==============================================================================
template <typename Scalar>
void TriMesh<Scalar>::normalize_triangle_normals()
{
  for (auto& normal : m_triangle_normals) {
    normal.normalize();
  }
}

} // namespace math
} // namespace dart
