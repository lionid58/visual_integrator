#include "visual_integrator/visualization/voxel.hh"

namespace Voxels
{
    glm::vec3 VoxelObject::get_position() {
        return position;
    }

    const std::vector<Voxel>& VoxelObject::get_grid() {
        return const_cast<const std::vector<Voxel>&>(voxels_grid);
    }

    VoxelObject::VoxelObject(std::function<bool(glm::vec3)> predicate, std::array<double,6> area, double vox_size, glm::vec3 pos) {
        size_t x_count, y_count, z_count;
        x_count = static_cast<size_t>((area[1] - area[0]) / vox_size) + 1;
        y_count = static_cast<size_t>((area[3] - area[2]) / vox_size) + 1;
        z_count = static_cast<size_t>((area[5] - area[4]) / vox_size) + 1;
        voxels_grid.reserve(x_count*y_count*z_count);
        dimention_size = glm::vec3(x_count, y_count, z_count);
        double vox_rad = vox_size / 2;
        for(size_t i = 0; i < x_count; i++) {
            for(size_t j = 0; j < y_count;j++) {
                for(size_t k = 0; k < z_count; k++) {
                    glm::vec3 curent_pos {
                        i*vox_size - vox_rad,
                        j*vox_size - vox_rad,
                        k*vox_size-vox_rad
                    };
                    voxels_grid.at(i + j * (x_count + k * y_count)) = Voxel{predicate(curent_pos)};
                }
            }
        } 
    }
} // namespace Voxels
