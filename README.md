# Visual Integrator

C++20/OpenGL application built with CMake and dependencies managed by vcpkg.

## Build and test

```bash
cmake --preset default
cmake --build --preset default
ctest --preset default
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
