#pragma once

#include <array>
#include <cstddef>
#include <glm/vec3.hpp>
#include <vector>

namespace visual_integrator {

struct Grid;

enum Axis { k_x, k_y, k_z };

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

  void add_triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c);

  TriangelMesh(std::vector<glm::vec3> vertics_,
               std::vector<Triangel> triangels_);
};

std::vector<TriangelMesh> AreaCuting(const TriangelMesh&, const Grid&, Axis);
}  // namespace visual_integrator