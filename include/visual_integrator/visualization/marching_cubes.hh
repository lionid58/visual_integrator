<<<<<<< HEAD
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
=======
#include <vector>
#include <functional>
#include <glm/vec3.hpp>
#include <vector>

namespace visual_integrator {

    enum Location:bool {
        outside,
        inside
    };
}

ParserFunc::ParserFunc() {
  parserF.DefineVar("x", &var_x);
  parserF.DefineVar("y", &var_y);
  parserF.DefineVar("z", &var_z);
}

double ParserFunc::Evaluate(double x, double y, double z) {
  var_x = x;
  var_y = y;
  var_z = z;
  try {
    return parserF.Eval();
  } catch (mu::Parser::exception_type& e) {
    std::cout << e.GetMsg() << std::endl;
  }
}


struct Grid {
  glm::dvec3 step;
  glm::dvec3 origin;
  glm::uvec3 dimensions;

  std::vector<double> valuse;
  std::vector<Location> locations;

        Grid::Grid(glm::dvec3 steps, glm::dvec3 origin_of_area, 
            glm::dvec3 area_sizes, std::function<double(glm::dvec3)> impSurfFunc);
    };
} // namespace visual_integrator
>>>>>>> a365b78 (feat: Voxels remove, add Grid for marching cubes)
