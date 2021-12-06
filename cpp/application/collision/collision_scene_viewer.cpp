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

#include "collision_scene_viewer.hpp"

//==============================================================================
CollisionSceneViewer::CollisionSceneViewer(
    const dart::gui::ViewerConfig& config)
  : dart::gui::Viewer(config)
{
  m_collision_scene_node = std::make_unique<CollisionSceneNode>(this);
  m_root_node->add_child(m_collision_scene_node.get());

  // Grid
  m_grid_node = std::make_unique<dart::gui::GridNode>();
  m_root_node->add_child(m_grid_node.get());
}

//==============================================================================
CollisionSceneViewer::~CollisionSceneViewer()
{
  // Do nothing
}

//==============================================================================
void CollisionSceneViewer::set_collision_scene(
    dart::collision::Scene<double>* scene)
{
  m_collision_scene_node->set_scene(scene);
}

//==============================================================================
void CollisionSceneViewer::grid_on()
{
  m_root_node->add_child(m_grid_node.get());
}

//==============================================================================
void CollisionSceneViewer::grid_off()
{
  m_root_node->remove_child(m_grid_node.get());
}
