# USC ITP 380 — C++ Game Programming Labs

A collection of C++ game-programming coursework from **USC ITP 380**, organized as twelve lab projects. The original gameplay and engine source files have been preserved; this repository is a cleaned **source archive**, not a newly refactored game or a verified release build.

> **Coursework and rights notice:** The lab framework and some files contain original copyright notices (including notices naming Sanjay Madhav). Some assets and third-party libraries may have separate ownership or license terms. Those notices have been retained. Please confirm the course's sharing rules and asset redistribution rights **before making this repository public**. A private repository is a prudent starting point. Do not represent all starter/framework code or bundled assets as solely your original work.

## Contents

| Location | What it contains |
| --- | --- |
| `Lab01/` – `Lab04/` | Early C++/SDL game labs, including player movement and collision exercises |
| `Lab05/` | Pac-Man-style mechanics and ghost AI |
| `Lab06/` | 2D game systems, pathfinding and enemy AI |
| `Lab07/` | 3D rendering, UI and projectile systems |
| `Lab08/` | 3D racing, track geometry and height maps |
| `Lab09/` – `Lab12/` | Portal/puzzle game systems, including portals, level loading and additional gameplay interactions |
| `Libraries/` | Bundled SDL, GLEW and other third-party build dependencies |
| `Scripts/` | Existing build and diagnostic helper scripts |
| `CMakeLists.txt` | Top-level CMake configuration (C++17) |

Each lab has its own `CMakeLists.txt` and source files. Relevant `Assets/` and `Shaders/` directories are included alongside them.

## Build (macOS example)

Requires CMake, Ninja, and a compatible C++ toolchain. Library compatibility may depend on your macOS version and CPU architecture.

```bash
cmake -S . -B build -G Ninja
cmake --build build --target Lab12
cd build/Lab12
./Lab12
```

To build a different lab, replace `Lab12` with `Lab01` through `Lab12` as appropriate.

On Windows, use a suitable Visual Studio C++ toolchain with CMake, then build a selected target, for example:

```powershell
cmake -S . -B build
cmake --build build --config Debug --target Lab12
```

Run the resulting executable from its output directory so relative paths such as `Assets/...` can resolve. Some platform/library combinations may require additional setup. **This cleanup did not compile or playtest these projects.**

## Repository hygiene

- C++ source, CMake configuration, shader files, and required project assets were left unchanged.
- Generated `build/` output, the old `.git/` history, macOS `__MACOSX` and `.DS_Store` metadata, editor-specific `.vs/` and `.vscode/` settings, and course-specific GitHub Actions workflows were omitted from the import package.
- No blanket open-source `LICENSE` has been added because the archive includes course-provided and third-party material.

See [PUBLISH_TO_GITHUB.md](PUBLISH_TO_GITHUB.md) for a no-terminal GitHub Desktop walkthrough.

## Attribution

Developed as part of USC ITP 380 coursework. Original copyright notices within the files have been preserved. The course framework, libraries, and media assets may have distinct authors or owners. If you plan to use the project publicly as a portfolio artifact, identify **your specific contributions** and ensure you have permission to share the source and assets.
