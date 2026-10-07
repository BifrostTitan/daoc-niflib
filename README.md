# Niflib++

Niflib++ is the standalone C++ library being built from the existing niflib.net codebase maintained by dol-leodagan for later use by Unreal Engine 5. It has no Unreal Engine dependency; the public API uses standard C++ types, and the library can be built as a static library with CMake.

## Build

```powershell
cmake -S . -B build
cmake --build build --config Release
```

## Port Status

The C++ library now declares equivalents for all 146 public C# classes and enums. Its built-in block factory registers all 100 transitive `NiObject` types. The port includes NIF file/header/footer parsing, typed reference resolution, scene graph, geometry and strips, materials and textures, skinning, morph targets, animation tracks/controllers, particles, collision modifiers, and extra data.

The parser retains the source library's format limits: NIF versions 20.0.0.4 and newer are rejected. Unknown block types fail explicitly. The standalone library uses standard C++17 types.

The API is in `include/niflib`. Consumers can link the `Niflib::niflib` CMake target or include the source and public headers in an Unreal module.

To build and run the focused tests, configure with `-DNIFLIB_BUILD_TESTS=ON`, build, and run `ctest` against the build directory.
