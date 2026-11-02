#include "visual_integrator/visualization/marching_cubes.hh"
#include <cstdint>
namespace visual_integrator
{
     Grid::Grid(glm::dvec3 steps, glm::dvec3 origin_of_area, 
            glm::dvec3 area_sizes, std::function<double(glm::dvec3)> impSurfFunc) {
                step = steps;
                origin = origin_of_area;
                dimensions = {static_cast<uint32_t>(area_sizes.x / steps.x),
                    static_cast<uint32_t>(area_sizes.y / steps.y),
                    static_cast<uint32_t>(area_sizes.z / steps.z)};
                for (size_t i = 0; i < dimensions.x; i++) {
                    for (size_t j = 0; j < dimensions.y; j++) {
                        for (size_t k = 0;k < dimensions.z; k++ ) {
                            size_t position = (i*dimensions.x + j) * dimensions.y + k; 
                            valuse.at(position) = impSurfFunc(glm::dvec3{origin.x + i*step.x,
                            origin.y + j*step.y,
                            origin.z + k * step.z});
                            locations.at(position) = Location{valuse.at(position) < 0};
                        }
                    }
                }
                
            }
} // namespace visual_integrator
