# CloudWalk3D

CloudWalk3D is an interactive 3D point cloud visualization tool built with modern OpenGL which allows it to run smoothly on modest hardware.

Most point cloud viewers use a CAD-style orbit camera, fixed around a single pivot point. CloudWalk3D instead puts you **inside** the data, letting you walk and fly through it with intuitive game-like controls — making it easier to inspect fine detail from any angle without constantly fighting the camera.

CloudWalk3D supports the `.e57` file format, which is a widely used export format for handheld laser scanners. This makes it well suited in applications like remote industrial inspections and topographic surveys of hard-to-reach terrain. It is also a fun way to explore 3D scans. Example point cloud files are included for anyone who wants to try it without their own data.

## Features

- **Point cloud loading** — `.csv` and `.e57` formats, with automatic camera and floor framing based on the loaded data's extents
- **Procedural example data** — a pseudo-random smooth height-field grid, generated via layered value noise (FBM), for quick testing without needing a real dataset
- **Free-fly camera** — An intuitive control scheme which will be instantly familiar to anyone who has ever touched a game like "Minecraft".
- **Custom UI** — startup screen (file picker, info panel), pause menu, and an in-scene HUD, all rendered with a hand-rolled text/button system (FreeType for glyphs, GLFW for windowing/input)

## Controls

| Input | Action |
|---|---|
| `W` / `A` / `S` / `D` | Move horizontally |
| `Space` | Move up |
| `Left Shift` | Move down |
| `Mouse1` (hold left button) | Orbit / look around |
| `Mouse2` (press right button) | Reset camera to default pose |
| `Esc` | Pause / resume |
| `Q` | Quit |

## Building & Distribution Notes

### Prerequisites
- **MSYS2** with the **MINGW64** toolchain (GCC, not MSVC)
- **CMake** version 3.24 or newer
- **Internet access at configure time** — most dependencies are fetched from source via CMake's `FetchContent`, so a first-time configure needs to reach GitHub

Run in the `MSYS2 MINGW64` shell:
```bash
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake git
```
### Configure and build
Important: 

```bash
cmake --preset default
cmake --build build --parallel
```

Because nearly everything compiles from source, a first-time build (or any build after deleting `build/`) is noticeably slower than an incremental one — expect it to take a while, particularly Xerces-C and FreeType. Subsequent builds are fast, since CMake and the underlying build tool only recompile what's actually changed.

That said, if you're just running the provided executable (which is the intended usage for most people), none of this applies. The release binary is statically linked and should run standalone on any machine with Windows 11 with no additional setup, dependencies, or compiler required.

## Running the Executable

The executable expects to find its `shaders`, `textures`, and `fonts` folders alongside it — these are copied next to the `.exe` at build time and shipped in the same zip. If you move `CathodeAutopsy.exe` out of that folder on its own, it won't be able to locate these resources and will fail to run correctly. Keep the full extracted folder structure intact and run the executable from within it.

> **Note:** In the future, these assets will be embedded directly into the binary as byte data, which would remove this restriction — but for now, treat the zip's folder layout as required.

## Dependencies
 
| Library | How it's obtained | Purpose |
|---|---|---|
| [GLFW](https://www.glfw.org/) | CMake `FetchContent` | Windowing & input |
| [GLM](https://github.com/g-truc/glm) | CMake `FetchContent` | Math (vectors, matrices, transforms) |
| [FreeType](https://freetype.org/) | CMake `FetchContent` | Font rasterization for UI text |
| [libE57Format](https://github.com/asmaloney/libE57Format) | CMake `FetchContent` | Reading/writing `.e57` point cloud files |
| [Xerces-C](https://xerces.apache.org/xerces-c/) | CMake `FetchContent` | XML parsing (used internally by libE57Format) |
| [glad](https://glad.dav1d.de/) | Vendored source (`external/`) | OpenGL function loading |
| [stb_image](https://github.com/nothings/stb) | Vendored source (`external/`) | Texture loading |
| [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/) | Vendored source (`external/`) | Native "open file" dialog |

## Project structure
 
```
src/
├── core/          Application entry point, main loop, path resolution
├── data/          Grid generation, point cloud loading, primitive geometry
├── platform/      Windowing (GLFW wrapper) and input handling
├── rendering/     Camera, shaders, renderer, text rendering, textures
└── ui/            Startup/pause/running screens and shared UI geometry helpers

include/           
└── ...            Headers for src/

external/
└── source/        Vendored single-file / small dependencies
```

## Known limitations

- **Up-axis convention**: E57 and CSV point cloud files don't universally agree on whether Z or Y is "up." `PointCloud::set_source_is_z_up()` lets you specify this per load, but it isn't yet exposed as a UI toggle — for now it must be set in code before calling `load()`.

- **E57 support required a MinGW-specific fix upstream**: stock **libE57Format** had a bug where files were opened without `O_BINARY`, causing Windows to corrupt binary point data via text-mode newline translation. A fix has been proposed and later merged, but since the current release predates this, `CMakeLists.txt` pins `libE57Format` to a commit that includes the fix. Once a release containing it is cut, this can move to a normal version tag.

## License

TBD
