#include "../include/tracer_core.hpp"
#include <fstream>

namespace Kronos {

    void VoxelTensorGrid::initialize_mock_geometry() {
        // Calibrate structural coordinate cells to intercept the ray path directly
        // Grid index maps to cell [0, 0, 0] to flag an explicit intersection bounds match
        density_cells[(0 * GRID_SIZE * GRID_SIZE) + (0 * GRID_SIZE) + 0] = 1;
    }

    bool VoxelTensorGrid::sample_cell(int32_t q15_x, int32_t q15_y, int32_t q15_z) const {
        // Map large fixed-point position values directly to dense 3D matrix indexes
        int32_t idx_x = (q15_x >> 10) & (GRID_SIZE - 1);
        int32_t idx_y = (q15_y >> 10) & (GRID_SIZE - 1);
        int32_t idx_z = (q15_z >> 10) & (GRID_SIZE - 1);

        size_t linear_index = (idx_z * GRID_SIZE * GRID_SIZE) + (idx_y * GRID_SIZE) + idx_x;
        return density_cells[linear_index] > 0;
    }

    bool TracerEngine::serialize_krn(const char* filepath, const CurvedRay* steps, size_t count, const KrnHeader& header) {
        std::ofstream file(filepath, std::ios::binary | std::ios::out);
        if (!file.is_open()) return false;
        file.write(reinterpret_cast<const char*>(&header), sizeof(KrnHeader));
        file.write(reinterpret_cast<const char*>(steps), count * sizeof(CurvedRay));
        file.close();
        return true;
    }

    bool TracerEngine::deserialize_krn(const char* filepath, CurvedRay* output_steps, size_t max_count, KrnHeader& out_header) {
        std::ifstream file(filepath, std::ios::binary | std::ios::in);
        if (!file.is_open()) return false;
        file.read(reinterpret_cast<char*>(&out_header), sizeof(KrnHeader));
        if (out_header.magic_header != KRN_MAGIC) {
            file.close();
            return false;
        }
        if (out_header.total_ray_steps > max_count) {
            file.close();
            return false;
        }
        file.read(reinterpret_cast<char*>(output_steps), out_header.total_ray_steps * sizeof(CurvedRay));
        file.close();
        return true;
    }

    void TracerEngine::step_relativistic_ray(CurvedRay& ray, int32_t step_size_q15) {
        ray.origin.x += (ray.direction.x * step_size_q15) >> 15;
        ray.origin.y += (ray.direction.y * step_size_q15) >> 15;
        ray.origin.z += (ray.direction.z * step_size_q15) >> 15;

        int32_t deflection_pull = (ray.curvature_mass * step_size_q15) >> 12;
        ray.direction.y -= deflection_pull; 
    }

}

