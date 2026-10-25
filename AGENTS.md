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

Use C++20 and keep compiler warnings (`-Wall -Wextra -Wpedantic`) clean. `.clang-format` defines Google style, two-space indentation, a 100-column limit, left-aligned pointers, and case-sensitive include sorting. Use `snake_case` for files and functions, `PascalCase` for types, and `UPPER_SNAKE_CASE` for constants or macros. Prefer small translation units and expose interfaces through `include/` only when they are shared.

## Testing Guidelines

Tests use GoogleTest and `gtest_discover_tests`. Add focused `TEST` blocks to files named `*_test.cpp` under `tests/`, and register new test source files in `CMakeLists.txt`. Cover new behavior and regressions; no numeric coverage threshold is currently configured. Run the default build and `ctest --preset default` before submitting changes.

## Commit & Pull Request Guidelines

Git history is not available in this checkout. Use short, imperative commit subjects such as `Add camera input handling`; keep unrelated changes separate. Pull requests should explain the change and motivation, list verification commands, and link relevant issues. Include screenshots or recordings for rendering or UI changes, and note platform, GPU, or driver-specific behavior.

## Configuration Notes

Dependencies are managed through vcpkg. `CMakePresets.json` currently references a machine-specific vcpkg toolchain path; update it locally or provide `CMAKE_TOOLCHAIN_FILE` when configuring on another system. Do not commit credentials, local paths, or generated dependency trees.
