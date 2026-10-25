# Visual Integrator

C++20/OpenGL application built with CMake and dependencies managed by vcpkg.

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
