# Visual Integrator

C++20/OpenGL application built with CMake and dependencies managed by vcpkg.

## Project structure

- `src/main.cpp`, `src/app.cpp`: entry point and application orchestration.
- `src/platform/`: GLFW window and context ownership.
- `src/rendering/`: OpenGL rendering.
- `src/visualization/`: voxel and geometric data for visualization.
- `include/visual_integrator/`: shared headers mirroring the source modules.
- `tests/`: GoogleTest tests; `cmake/`: build helpers.

The `ui`, `parsing`, and `math` modules described in `struct.md` will be added
when their behavior is implemented. The primitive files are unfinished drafts
and are not compiled yet.

## Build and test

Set `VCPKG_ROOT` to the directory where vcpkg is installed. On Linux:

```bash
export VCPKG_ROOT="$HOME/.local/share/vcpkg"
cmake --preset default
cmake --build --preset default
ctest --preset default
```

On Windows, run the following from a Visual Studio Developer PowerShell so that
the MSVC compiler is available:

```powershell
$env:VCPKG_ROOT = "C:\src\vcpkg"
cmake --preset windows-debug
cmake --build --preset windows-debug
ctest --preset windows-debug
```

## Formatting

```bash
cmake --build --preset format
cmake --build --preset format-check
```

For an optimized build:

```bash
cmake --preset release
cmake --build --preset release
```

The equivalent optimized Windows preset is `windows-release`.

## Marching Cubes topology conventions

The public tables are in
`include/visual_integrator/visualization/marching_cubes_tables.hh`, namespace
`visual_integrator::marching_cubes`. They follow AMD TressFX, including its XZ
lower ring and Y upper direction. Do not swap Y/Z or reverse triangle indices.

Corner offsets `(x, y, z)` in `CORNER_OFFSETS`:

| Corner | Offset |
| --- | --- |
| 0 | (0, 0, 0) |
| 1 | (1, 0, 0) |
| 2 | (1, 0, 1) |
| 3 | (0, 0, 1) |
| 4 | (0, 1, 0) |
| 5 | (1, 1, 0) |
| 6 | (1, 1, 1) |
| 7 | (0, 1, 1) |

Edges in `EDGE_CORNERS`, with ordered endpoint pairs:

| Edge | Corners |
| --- | --- |
| 0 | (0, 1) |
| 1 | (1, 2) |
| 2 | (2, 3) |
| 3 | (3, 0) |
| 4 | (4, 5) |
| 5 | (5, 6) |
| 6 | (6, 7) |
| 7 | (7, 4) |
| 8 | (0, 4) |
| 9 | (1, 5) |
| 10 | (2, 6) |
| 11 | (3, 7) |

`VoxelObject` stores occupancy of grid cells: `space` is empty and `materia` is
filled. A future polygonizer must define how cell samples form a Marching Cubes
cube (for example, eight neighboring cell centers); these offsets specify the
relative sample order, not eight occupancy values within one voxel.
Use configuration bit `i` for a filled sample at corner `i`. This corresponds
to TressFX's `scalar[i] < isolevel` convention; it is not a direct cast of a
`Voxel` value into the scalar field compared against an isolevel.

`EDGE_MASKS[configuration]` is a 12-bit mask: bit `e` marks an edge whose
endpoints have different occupancy. `TRIANGLE_EDGES[configuration]` contains
16 signed entries. Read consecutive triples `(e0, e1, e2)` until the first
`-1`; all remaining entries are `-1`. Each triple indexes interpolated edge
vertices, not cube corners. There are at most five triangles per configuration.
Preserve the listed vertex order: TressFX computes the normal as
`cross(v1 - v0, v2 - v0)`. With the coordinates above and filled configuration
bits, case 1 is `(0, 8, 3)` and its normal points away from filled corner 0;
case 254 is `(0, 3, 8)`. Cases 0 and 255 contain no triangles.

These are classic Marching Cubes tables. They do not implement an asymptotic
decider or MC33 and do not guarantee consistent topology for ambiguous cases.
Do not derive complementary triangle rows by reversing indices: ambiguous
complementary cases can have different triangulations.

Sources and verification (2026-11-01):

- [AMD TressFX tables](https://github.com/GPUOpen-Effects/TressFX/blob/master/src/MarchingCubesTables.h):
  source of all 256 edge masks and 256 triangle rows; MIT notice retained in
  the header.
- [AMD TressFX shader](https://github.com/GPUOpen-Effects/TressFX/blob/master/src/Shaders/TressFXMarchingCubes.hlsl):
  source of corner offsets, edge endpoints, configuration bits and vertex order.
- [Three.js MarchingCubes](https://github.com/mrdoob/three.js/blob/dev/examples/jsm/objects/MarchingCubes.js):
  additional implementation checked element by element; all 256 masks and
  4096 triangle entries match exactly. Coordinate conventions were taken from
  the TressFX shader, not inferred from matching tables.
- GoogleTest checks all 256 configurations for crossing-edge masks, valid edge
  indices, complete triangles, distinct triangle vertices, and sentinel padding;
  literal reference cases cover empty/full cubes, opposite winding and five triangles.
