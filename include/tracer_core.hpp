#ifndef TRACER_CORE_HPP
#define TRACER_CORE_HPP

#include <cstdint>
#include <cstddef>

namespace Kronos {

    // Cache-aligned coordinates for vector math processing
    struct alignas(16) Vector3Q15 {
        int32_t x;
        int32_t y;
        int32_t z;
    };

    // Cache-aligned non-Euclidean Ray primitive structure
    struct alignas(64) CurvedRay {
        Vector3Q15 origin;      // Q15 Fixed-Point Position spatial coordinates
        Vector3Q15 direction;   // Q15 Fixed-Point Direction vector components
        int32_t curvature_mass; // Distorting mass magnitude modifier
    };

    // Cache-aligned custom file container header
    struct alignas(64) KrnHeader {
        uint32_t magic_header;    // Must resolve to Little Endian 'KRN!'
        uint32_t spatial_version; // Target structural layout revision index
        uint32_t total_ray_steps; // Number of processed data points recorded
        uint32_t reserved_pad;    // Absolute 64-byte structural padding element
    };

    class TracerEngine {
    public:
        static constexpr uint32_t KRN_MAGIC = 0x214E524B; // 'KRN!' in Little Endian

        static bool serialize_krn(const char* filepath, const CurvedRay* steps, size_t count, const KrnHeader& header);
        static bool deserialize_krn(const char* filepath, CurvedRay* output_steps, size_t max_count, KrnHeader& out_header);
        
        // Non-Euclidean single integration step calculation
        static void step_relativistic_ray(CurvedRay& ray, int32_t step_size_q15);
    };

}

#endif

