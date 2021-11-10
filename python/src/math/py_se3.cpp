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

#include <pybind11/pybind11.h>

#include "dart/math/all.hpp"

namespace py = pybind11;

namespace dart::python {

void py_se3(py::module& m)
{
  auto se3 = py::class_<math::SE3d>(m, "SE3")
                 .def(py::init<>())
                 .def("set_identity", &math::SE3d::set_identity)
                 .def(
                     "is_identity",
                     &math::SE3d::is_identity,
                     py::arg("tolerance") = math::eps<double>())
                 .def("set_random", &math::SE3d::set_random)
                 .def_static("Identity", &math::SE3d::Identity)
                 .def_static("Random", &math::SE3d::Random);

  auto se3_tangent = py::class_<math::SE3Tangentd>(m, "SE3Tangent")
                         .def(py::init<>())
                         .def(
                             "is_zero",
                             &math::SE3Tangentd::is_zero,
                             py::arg("tolerance") = math::eps<double>());

  auto se3_cotangent = py::class_<math::SE3Cotangentd>(m, "SE3Cotangent")
                           .def(py::init<>())
                           .def(
                               "is_zero",
                               &math::SE3Cotangentd::is_zero,
                               py::arg("tolerance") = math::eps<double>());

  se3.attr("Tangent") = se3_tangent;
  se3.attr("Cotangent") = se3_cotangent;
}

} // namespace dart::python