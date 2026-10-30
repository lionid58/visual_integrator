#include <vector>
#include <array>
#include <cstdint>
#include <glm/vec3.hpp>
#include <functional>


namespace Voxels {

    enum Voxel : bool {
        space,
        materia,
    };

    class VoxelObject {
    private:
        glm::vec3 position;
        std::vector<Voxel> voxels_grid;
        double voxel_size;
        glm::vec3 dimention_size;

    public:
        VoxelObject(std::function<bool(glm::vec3)> predicate, std::array<double,6> area, double vox_size, glm::vec3 pos);
        void update_voxel_size(double new_size);
        const std::vector<Voxel>& get_grid();
        glm::vec3 get_position();
        double get_voxel_size();
    };
} // namespace Voxels