#include "visual_integrator/visualization/primitives.hh"
#include <utility>


namespace visual_integrator {
    TriangelMesh::TriangelMesh(std::vector<glm::vec3> vertics_, std::vector<Triangel> triangels_)
    : vertics(std::move(vertics_)), triangels(std::move(triangels_))
    {}
}