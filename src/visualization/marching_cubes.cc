#include "visual_integrator/visualization/marching_cubes.hh"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

#include "visual_integrator/visualization/marching_cubes_tables.hh"

namespace visual_integrator {

Grid::Grid(glm::dvec3 steps, glm::dvec3 origin_of_area, glm::dvec3 area_sizes,
           std::function<double(glm::dvec3)> impSurfFunc) {
  step = steps;
  origin = origin_of_area;
  size_t count = 1;
  for (int axis = 0; axis < 3; ++axis) {
    if (!std::isfinite(step[axis]) || step[axis] <= 0 ||
        !std::isfinite(origin[axis]) || !std::isfinite(area_sizes[axis]) ||
        area_sizes[axis] < 0 ||
        !std::isfinite(origin[axis] + area_sizes[axis])) {
      throw std::invalid_argument(
          "Grid requires finite geometry, positive steps and nonnegative "
          "sizes");
    }
    const double samples = std::floor(area_sizes[axis] / step[axis]) + 1;
    if (!std::isfinite(samples) ||
        samples > std::numeric_limits<uint32_t>::max()) {
      throw std::length_error("Grid dimension is too large");
    }
    dimensions[axis] = static_cast<uint32_t>(samples);
    if (count > valuse.max_size() / dimensions[axis]) {
      throw std::length_error("Grid sample count is too large");
    }
    count *= dimensions[axis];
  }
  valuse.resize(count);
  locations.resize(valuse.size());
  for (size_t i = 0; i < dimensions.x; i++) {
    for (size_t j = 0; j < dimensions.y; j++) {
      for (size_t k = 0; k < dimensions.z; k++) {
        size_t position = (i * dimensions.y + j) * dimensions.z + k;
        valuse.at(position) =
            impSurfFunc(glm::dvec3{origin.x + i * step.x, origin.y + j * step.y,
                                   origin.z + k * step.z});
        if (!std::isfinite(valuse.at(position))) {
          throw std::invalid_argument("Grid samples must be finite");
        }
        locations.at(position) = Location{valuse.at(position) < 0};
      }
    }
  }
}

TriangelMesh polygonize(const Grid& grid, double isolevel) {
  if (!std::isfinite(isolevel))
    throw std::invalid_argument("Isolevel must be finite");
  size_t count = 1;
  for (int axis = 0; axis < 3; ++axis) {
    if (grid.dimensions[axis] == 0 || !std::isfinite(grid.origin[axis]) ||
        !std::isfinite(grid.step[axis]) || grid.step[axis] <= 0 ||
        !std::isfinite(grid.origin[axis] +
                       (grid.dimensions[axis] - 1) * grid.step[axis]) ||
        count > grid.valuse.max_size() / grid.dimensions[axis]) {
      throw std::invalid_argument("Invalid grid geometry");
    }
    count *= grid.dimensions[axis];
  }
  if (count != grid.valuse.size())
    throw std::invalid_argument("Invalid grid sample count");
  for (double value : grid.valuse) {
    if (!std::isfinite(value))
      throw std::invalid_argument("Grid samples must be finite");
  }
  std::vector<glm::vec3> vertices;
  std::vector<Triangel> triangles;
  for (size_t x = 0; x + 1 < grid.dimensions.x; ++x) {
    for (size_t y = 0; y + 1 < grid.dimensions.y; ++y) {
      for (size_t z = 0; z + 1 < grid.dimensions.z; ++z) {
        std::array<double, 8> values;
        std::array<glm::dvec3, 8> positions;
        unsigned configuration = 0;
        for (size_t corner = 0; corner < 8; ++corner) {
          const auto& offset = marching_cubes::CORNER_OFFSETS[corner];
          const size_t i = x + offset[0];
          const size_t j = y + offset[1];
          const size_t k = z + offset[2];
          values[corner] = grid.valuse.at(
              (i * grid.dimensions.y + j) * grid.dimensions.z + k);
          positions[corner] = grid.origin + glm::dvec3(i, j, k) * grid.step;
          if (values[corner] < isolevel) configuration |= 1u << corner;
        }
        std::array<size_t, 12> edge_vertices{};
        for (size_t edge = 0; edge < 12; ++edge) {
          if (!(marching_cubes::EDGE_MASKS[configuration] & (1u << edge)))
            continue;
          const auto& endpoints = marching_cubes::EDGE_CORNERS[edge];
          const auto a = endpoints[0];
          const auto b = endpoints[1];
          const double scale = std::max(
              {std::abs(values[a]), std::abs(values[b]), std::abs(isolevel)});
          const double scaled_a = values[a] / scale;
          const double scaled_b = values[b] / scale;
          const double t =
              (isolevel / scale - scaled_a) / (scaled_b - scaled_a);
          edge_vertices[edge] = vertices.size();
          vertices.emplace_back(positions[a] +
                                t * (positions[b] - positions[a]));
        }
        const auto& row = marching_cubes::TRIANGLE_EDGES[configuration];
        for (size_t entry = 0; entry < row.size() && row[entry] != -1;
             entry += 3) {
          triangles.push_back(
              {{edge_vertices[row[entry]], edge_vertices[row[entry + 1]],
                edge_vertices[row[entry + 2]]}});
        }
      }
    }
  }
  return {std::move(vertices), std::move(triangles)};
}

}  // namespace visual_integrator
