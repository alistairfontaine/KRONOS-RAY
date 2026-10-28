# KRONOS-RAY: System Architecture

## 1. Non-Euclidean Vector Pipeline
KRONOS-RAY discards the standard linear ray-casting model (R(t) = O + tD) in favour of a discrete relativistic numerical integration step. The tracking pipeline maps positions through a variable pseudo-gravitational tensor field using integer-shifted vector calculus:

```text
  [ Initial CurvedRay Primitive ] ──> Stack Allocation
                                              │
                                              ▼
  [ Vector3Q15 Trajectory Step  ] ──> March Origin coordinates forward via relative components
                                              │
                                              ▼
  [ Tensor Bitwise Deflection   ] ──> Curvature mass multiplies step size to pull velocity downwards
                                              │
                                              ▼
  [ Voxel Tensor Grid Layer     ] ──> Linear 3D array cell lookup checks for intersection density
                                              │
                   ┌──────────────────────────┴──────────────────────────┐
                   ▼                                                     ▼
           [ Hit: FALSE ]                                         [ Hit: TRUE ]
     Continue Integration Step loop                        Terminate path evaluation loop
```

## 2. Spatial Indexing & Cache Optimization
- **Linearized Voxel Matrix:** The 3D grid layout is compressed into a 1D flat stack allocation array profile of size $N^3$ ($4 \times 4 \times 4 = 64$ elements) to eliminate row-pointer indirection overhead.
- **Cache-Line Boundary Constraints:** Primitives use explicit alignment constraints (`alignas(64)`) to map fields directly onto host CPU L1/L2 data cache line sizes, preventing memory fetching latencies.
- **Q15 Fixed-Point Domain:** All spatial processing calculations reject floating-point operational costs, avoiding hardware drift variants across local architectures.

