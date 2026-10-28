#ifndef TRACER_CORE_HPP
#define TRACER_CORE_HPP

#include <cstdint>
#include <cstddef>

namespace Kronos {

    struct alignas(16) Vector3Q15 {
        int32_t x;
        int32_t y;
        int32_t z;
    };

    struct alignas(64) CurvedRay {
        Vector3Q15 origin;      
        Vector3Q15 direction;   
        int32_t curvature_mass; 
    };

    struct alignas(64) KrnHeader {
        uint32_t magic_header;    
        uint32_t spatial_version; 
        uint32_t total_ray_steps; 
        uint32_t reserved_pad;    
    };

    // Phase 2 Invariant: Cache-aligned 3D Voxel Grid Matrix Container
    class alignas(64) VoxelTensorGrid {
    public:
        static constexpr size_t GRID_SIZE = 4; // 4x4x4 spatial bounds block
        uint8_t density_cells[GRID_SIZE * GRID_SIZE * GRID_SIZE]{0};

        VoxelTensorGrid() = default;
        void initialize_mock_geometry();
        bool sample_cell(int32_t q15_x, int32_t q15_y, int32_t q15_z) const;
    };

    class TracerEngine {
    public:
        static constexpr uint32_t KRN_MAGIC = 0x214E524B; 

        static bool serialize_krn(const char* filepath, const CurvedRay* steps, size_t count, const KrnHeader& header);
        static bool deserialize_krn(const char* filepath, CurvedRay* output_steps, size_t max_count, KrnHeader& out_header);
        static void step_relativistic_ray(CurvedRay& ray, int32_t step_size_q15);
    };

}

#endif

