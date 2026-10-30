#include <glm/vec3.hpp>
#include <vector>
#include <array>


namespace visual_integrator {
    struct Triangel
    {
        std::array<std::uint8_t,3> vertics;
    };
    

    class TriangelMesh {
    private:
        std::vector<glm::vec3> vertics;
        std::vector<Triangel> triangels;
    public:
        TriangelMesh(std::vector<glm::vec3> vertics_, std::vector<Triangel> triangels_);
    };
}