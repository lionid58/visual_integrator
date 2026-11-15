#include <glm/vec3.hpp>
#include <vector>

#include "visual_integrator/mathcore/parser.h"
#include "visual_integrator/visualization/primitives.hh"

namespace visual_integrator {

enum Location : bool { outside, inside };

struct Grid {
  glm::dvec3 step;
  glm::dvec3 origin;
  glm::uvec3 dimensions;
  std::vector<double> valuse;
  std::vector<Location> locations;

  Grid(glm::dvec3 steps, glm::dvec3 origin_of_area, glm::dvec3 area_sizes,
       ParserFunc& impSurfFunc);
};

TriangelMesh polygonize(const Grid& grid, double isolevel = 0);
}  // namespace visual_integrator
