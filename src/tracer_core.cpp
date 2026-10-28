#include "../include/tracer_core.hpp"
#include <fstream>

namespace Kronos {

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
        // Linearly march the ray origin vectors along their current trajectories
        ray.origin.x += (ray.direction.x * step_size_q15) >> 15;
        ray.origin.y += (ray.direction.y * step_size_q15) >> 15;
        ray.origin.z += (ray.direction.z * step_size_q15) >> 15;

        // Apply a non-Euclidean bitwise deflection pull downwards towards a simulated mass center
        // Bends the ray path non-linearly using pure integer shifts to avoid floating-point drift
        int32_t deflection_pull = (ray.curvature_mass * step_size_q15) >> 12;
        ray.direction.y -= deflection_pull; 
    }

}

