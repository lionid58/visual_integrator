#include <functional>
#include <glm/vec3.hpp>
#include <vector>

namespace visual_integrator {

enum Location : bool { outside, inside };

struct Grid {
  glm::dvec3 step;
  glm::dvec3 origin;
  glm::uvec3 dimensions;

  std::vector<double> valuse;
  std::vector<Location> locations;

  Grid::Grid(glm::dvec3 steps, glm::dvec3 origin_of_area, glm::dvec3 area_sizes,
             std::function<double(glm::dvec3)> impSurfFunc);
};
}  // namespace visual_integrator