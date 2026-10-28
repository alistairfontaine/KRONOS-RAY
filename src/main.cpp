#include "../include/tracer_core.hpp"
#include <iostream>

int main() {
    std::cout << "[KRONOS-RAY]: Initializing non-Euclidean path-tracing matrix..." << std::endl;

    // Allocate spatial grid geometry parameters
    Kronos::VoxelTensorGrid spatial_grid;
    spatial_grid.initialize_mock_geometry();

    alignas(64) Kronos::CurvedRay active_ray = {
        { 0, 0, 0 },        
        { 32768, 4096, 0 }, 
        8192                
    };

    constexpr size_t MAX_STEPS = 4;
    alignas(64) Kronos::CurvedRay path_history[MAX_STEPS];
    size_t recorded_steps = 0;

    std::cout << "[KRONOS-RAY]: Step-marching non-linear rays through voxel structures..." << std::endl;
    for (size_t i = 0; i < MAX_STEPS; ++i) {
        Kronos::TracerEngine::step_relativistic_ray(active_ray, 1024);
        path_history[i] = active_ray;
        recorded_steps++;

        bool hit_detected = spatial_grid.sample_cell(active_ray.origin.x, active_ray.origin.y, active_ray.origin.z);
        
        std::cout << "  Step [" << i << "]: Pos(" 
                  << path_history[i].origin.x << ", " 
                  << path_history[i].origin.y << ", " 
                  << path_history[i].origin.z << ") -> Hit: " 
                  << (hit_detected ? "TRUE (Voxel Intersection)" : "FALSE") << std::endl;

        if (hit_detected) {
            std::cout << "[KRONOS-RAY]: Path trace terminated at spatial interception boundary." << std::endl;
            break;
        }
    }

    Kronos::KrnHeader config_header;
    config_header.magic_header = Kronos::TracerEngine::KRN_MAGIC;
    config_header.spatial_version = 0x01;
    config_header.total_ray_steps = static_cast<uint32_t>(recorded_steps);
    config_header.reserved_pad = 0x00;

    if (Kronos::TracerEngine::serialize_krn("spatial.krn", path_history, recorded_steps, config_header)) {
        std::cout << "[KRONOS-RAY]: Real-time intersection matrix serialized cleanly." << std::endl;
    }

    return 0;
}

