#pragma once

#include <array>
#include <cstddef>
#include <glm/vec3.hpp>
#include <vector>

namespace visual_integrator {
struct Triangel {
  std::array<std::size_t, 3> vertics;
};

class TriangelMesh {
 private:
  std::vector<glm::vec3> vertics;
  std::vector<Triangel> triangels;

 public:
  const std::vector<glm::vec3>& vertices() const { return vertics; }
  const std::vector<Triangel>& triangles() const { return triangels; }

  TriangelMesh(std::vector<glm::vec3> vertics_,
               std::vector<Triangel> triangels_);
};
}  // namespace visual_integrator