#pragma once

#include <glm/vec3.hpp>
#include <string>
#include <vector>

#include "visual_integrator/visualization/primitives.hh"

namespace visual_integrator {
struct AreaRequest {
  std::string expression;
  glm::dvec3 min;
  glm::dvec3 max;
  int cells = 32;
  Axis axis = k_x;
};
struct AreaResult {
  std::vector<TriangelMesh> slices;
  glm::dvec3 min;
  glm::dvec3 max;
};
AreaResult build_area(const AreaRequest& request);
}  // namespace visual_integrator
