# Repository Guidelines

## Project Structure & Module Organization

This is a C++20 OpenGL application built with CMake and Ninja. Application code lives in `src/`; place reusable public headers in `include/`. Tests belong in `tests/` and are registered through GoogleTest. CMake helpers live in `cmake/`, while dependency versions and features are declared in `vcpkg.json`. Keep generated files under `build/` (for example, `build/default`) and do not commit build output or `vcpkg_installed/`.

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
