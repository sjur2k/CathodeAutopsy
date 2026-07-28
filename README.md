## Building & Distribution Notes

Due to firewall issues on the company laptop used to develop this project, I couldn't download Visual Studio tools or use package managers reliably. Therefore, this project vendors several third-party libraries (GLFW, libE57Format, xerces-c, glm, FreeType, and others) either as prebuilt binaries or built locally as part of the CMake configuration.

If you're building from source, first-time builds may take a while due to compiling dependencies like xerces-c and FreeType, and you'll need a MinGW-w64 toolchain compatible with the rest of the project's build flags (static linking is used throughout). If you run into link errors, it's most likely a toolchain/ABI mismatch between your MinGW install and the vendored dependencies.

That said, if you're just running the provided executable (which is the intended usage) rather than building it yourself, none of this applies. The release binary is statically linked and should run standalone on any Windows 11 machine with no additional setup, dependencies, or compiler required.

## Running the Executable

The executable expects to find its `shaders`, `textures`, and `fonts` folders alongside it — these are copied next to the `.exe` at build time and shipped in the same zip. If you move `CathodeAutopsy.exe` out of that folder on its own, it won't be able to locate these resources and will fail to run correctly. Keep the full extracted folder structure intact and run the executable from within it.

> **Note:** In the future, these assets may be embedded directly into the binary as byte data, which would remove this restriction — but for now, treat the zip's folder layout as required.