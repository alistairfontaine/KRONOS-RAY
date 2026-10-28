#include "../include/tracer_core.hpp"
#include <iostream>

int main() {
    std::cout << "[KRONOS-RAY]: Initializing non-Euclidean path-tracing matrix..." << std::endl;

    // Set up a ray vector pointing forward with a high localized curvature mass component
    alignas(64) Kronos::CurvedRay active_ray = {
        { 0, 0, 0 },        // Origin (X, Y, Z)
        { 32768, 4096, 0 }, // Direction vectors (Q15 1.0, 0.125, 0.0)
        8192                // Curvature pull factor
    };

    constexpr size_t STEPS = 3;
    alignas(64) Kronos::CurvedRay path_history[STEPS];

    std::cout << "[KRONOS-RAY]: Executing relativistic integration steps..." << std::endl;
    for (size_t i = 0; i < STEPS; ++i) {
        Kronos::TracerEngine::step_relativistic_ray(active_ray, 1024); // March forward by delta step size
        path_history[i] = active_ray;
        
        std::cout << "  Step [" << i << "]: Pos(" 
                  << path_history[i].origin.x << ", " 
                  << path_history[i].origin.y << ", " 
                  << path_history[i].origin.z << ") Dir_Y(" 
                  << path_history[i].direction.y << ")" << std::endl;
    }

    Kronos::KrnHeader config_header;
    config_header.magic_header = Kronos::TracerEngine::KRN_MAGIC;
    config_header.spatial_version = 0x01;
    config_header.total_ray_steps = STEPS;
    config_header.reserved_pad = 0x00;

    const char* path_output = "spatial.krn";
    if (Kronos::TracerEngine::serialize_krn(path_output, path_history, STEPS, config_header)) {
        std::cout << "[KRONOS-RAY]: Non-Euclidean ray coordinates archived to: " << path_output << std::endl;
    }

    return 0;
}

