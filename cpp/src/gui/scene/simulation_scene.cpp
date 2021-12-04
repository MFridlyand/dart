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

#include "dart/gui/scene/simulation_scene.hpp"

#include "dart/common/logging.hpp"
#include "dart/common/macro.hpp"
#include "dart/gui/camera.hpp"
#include "dart/gui/raylib_include.hpp"
#include "dart/simulation/all.hpp"

namespace dart::gui {

//==============================================================================
struct SimulationScene::Implementation
{
  std::shared_ptr<simulation::World> world;
  std::unordered_set<CameraPtr> cameras;

  Implementation()
  {
    // Do nothing
  }
};

//==============================================================================
SimulationScene::SimulationScene() : m_impl(std::make_unique<Implementation>())
{
  // Create an empty world
  m_impl->world = simulation::World::Create();
}

//==============================================================================
SimulationScene::~SimulationScene()
{
  // Do nothing
}

//==============================================================================
bool SimulationScene::set_simulation_world(
    std::shared_ptr<simulation::World> world)
{
  if (world == nullptr) {
    return false;
  }

  m_impl->world = std::move(world);
  return true;
}

//==============================================================================
void SimulationScene::update()
{
  m_impl->world->update();
}

//==============================================================================
void SimulationScene::render()
{
  const auto physics_step_count = m_impl->world->get_step_count();

  raylib::DrawText(
      ("Physics steps: " + std::to_string(physics_step_count)).c_str(),
      10,
      10,
      20,
      raylib::DARKBROWN);
}

} // namespace dart::gui
