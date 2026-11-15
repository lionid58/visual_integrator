#include "visual_integrator/visualization/area_pipeline.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "visual_integrator/visualization/marching_cubes.hh"

namespace visual_integrator {
AreaResult build_area(const AreaRequest& request) {
  if (request.expression.empty() || request.cells < 2 || request.cells > 128 ||
      request.axis < k_x || request.axis > k_z)
    throw std::invalid_argument(
        "Invalid expression, resolution, or cutting axis");
  for (int i = 0; i < 3; ++i)
    if (!std::isfinite(request.min[i]) || !std::isfinite(request.max[i]) ||
        request.min[i] >= request.max[i] ||
        !std::isfinite(request.max[i] - request.min[i]))
      throw std::invalid_argument("Ranges must be finite and increasing");

  ParserFunc parser;
  parser.parserF.SetExpr(request.expression);
  const glm::dvec3 step = (request.max - request.min) / double(request.cells);
  Grid grid(step, request.min, request.max - request.min, parser);
  grid.dimensions = glm::uvec3(request.cells + 1);
  const size_t n = request.cells + 1;
  grid.valuse.resize(n * n * n);
  grid.locations.resize(n * n * n);
  for (size_t x = 0; x < n; ++x)
    for (size_t y = 0; y < n; ++y)
      for (size_t z = 0; z < n; ++z) {
        const size_t index = (x * n + y) * n + z;
        double value =
            parser.Evaluate(request.min + glm::dvec3(x, y, z) * step);
        if (!std::isfinite(value))
          throw std::invalid_argument("Function returned a non-finite value");
        if (x == 0 || y == 0 || z == 0 || x + 1 == n || y + 1 == n ||
            z + 1 == n)
          value = std::max(value, 0.0);
        grid.valuse[index] = value;
        grid.locations[index] = Location{value < 0};
      }
  auto mesh = polygonize(grid);
  const int axis = static_cast<int>(request.axis);
  grid.origin[axis] -= step[axis] * 0.001;
  grid.step[axis] *= 1.0 + 0.002 / request.cells;
  return {AreaCuting(mesh, grid, request.axis), request.min, request.max};
}
}  // namespace visual_integrator
