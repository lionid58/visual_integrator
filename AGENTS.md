# Repository Guidelines

## Project Structure & Module Organization

This is a C++20 OpenGL application built with CMake and Ninja. Application code lives in `src/`; place reusable public headers in `include/`. Tests belong in `tests/` and are registered through GoogleTest. CMake helpers live in `cmake/`, while dependency versions and features are declared in `vcpkg.json`. Keep generated files under `build/` (for example, `build/default`) and do not commit build output or `vcpkg_installed/`.

## Architecture & Data Flow

`struct.md` describes the intended architecture for visualizing three-dimensional integration:

1. `ui`: collect input and display controls, fields, and validation feedback.
2. `parsing`: turn input into a complete mathematical problem or return validation errors.
3. `math`: build the mathematical model and solve the complete problem.
4. `visualization`: prepare solution data for rendering and maintain camera/view transformations.
5. `rendering`: draw prepared data with OpenGL.

The input stages use a resettable queue of pending edits. Downstream computation consumes only complete, validated snapshots. Changes to the mathematical problem require a full recomputation of dependent results. Rotation and zoom reuse the existing solution and visualization data, updating view transformations without rerunning parsing or integration. Keep GLFW, OpenGL, and ImGui out of parsing and math interfaces.

Current implementation:

- `src/main.cpp`: entry point and top-level error reporting only.
- `src/app.cpp`: application orchestration and event/render loop.
- `src/platform/window.cpp`: GLFW initialization, monitor queries, window/context ownership, events, and buffer swapping.
- `src/rendering/renderer.cpp`: OpenGL loading, framebuffer viewport, and drawing.
- Shared interfaces mirror these modules under `include/visual_integrator/`.

The `ui`, `parsing`, `math`, and `visualization` modules are planned, not implemented. Add matching directories under `src/` and shared headers under `include/visual_integrator/` when behavior is introduced; avoid empty placeholder modules. Register new translation units explicitly in `CMakeLists.txt`.

`Window` currently owns the single application window and the GLFW lifetime. Create it on the main thread before the renderer, and destroy rendering resources before the window/context. Use RAII so exceptions release resources. Use GLFW APIs for platform integration. Window sizes use screen coordinates; rendering uses framebuffer dimensions for HiDPI support.

## Build, Test, and Development Commands

- `cmake --preset default` configures a Debug build, enables tests, and generates Ninja files.
- `cmake --build --preset default` compiles the application and test executable.
- `ctest --preset default` runs all discovered GoogleTest tests and prints failures.
- `cmake --build --preset format-check` checks formatting without changing files.
- `cmake --build --preset format` applies clang-format to C++ sources and headers.
- `cmake --preset release && cmake --build --preset release` creates an optimized build without tests.

The executable is written to `build/default/visual_integrator` for the default preset.

## Coding Style & Naming Conventions

Use test driven development.
Use C++20 and keep compiler warnings (`-Wall -Wextra -Wpedantic`) clean. `.clang-format` defines Google style, two-space indentation, a 100-column limit, left-aligned pointers, and case-sensitive include sorting. Use `snake_case` for files and functions, `PascalCase` for types, and `UPPER_SNAKE_CASE` for constants or macros. Prefer small translation units and expose interfaces through `include/` only when they are shared.

## Testing Guidelines

Tests use GoogleTest and `gtest_discover_tests`. Add focused `TEST` blocks to files named `*_test.cpp` under `tests/`, and register new test source files in `CMakeLists.txt`. Cover new behavior and regressions; no numeric coverage threshold is currently configured. Run the default build and `ctest --preset default` before submitting changes.

## Commit & Pull Request Guidelines

Git history is not available in this checkout. Use short, imperative commit subjects such as `Add camera input handling`; keep unrelated changes separate. Pull requests should explain the change and motivation, list verification commands, and link relevant issues. Include screenshots or recordings for rendering or UI changes, and note platform, GPU, or driver-specific behavior.

## Configuration Notes

Dependencies are managed through vcpkg. `CMakePresets.json` currently references a machine-specific vcpkg toolchain path; update it locally or provide `CMAKE_TOOLCHAIN_FILE` when configuring on another system. Do not commit credentials, local paths, or generated dependency trees.

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
