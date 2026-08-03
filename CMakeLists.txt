cmake_minimum_required (VERSION 3.10)

project (CathodeAutopsy)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glm CONFIG REQUIRED)

include(FetchContent)
FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG        VER-2-13-3
)

set(FT_DISABLE_ZLIB TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI TRUE CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(freetype)

add_library(glad STATIC external/source/glad/glad.c)
target_include_directories(glad SYSTEM PUBLIC external/source/glad)

add_library(tinyfiledialogs STATIC external/source/tinyfiledialogs/tinyfiledialogs.c)
target_include_directories(tinyfiledialogs SYSTEM PUBLIC external/source/tinyfiledialogs)
target_link_libraries(tinyfiledialogs PUBLIC comdlg32 ole32)

add_library(stb_image STATIC external/source/stb_image/stb_image.cpp)
target_include_directories(stb_image SYSTEM PUBLIC external/source/stb_image)

add_executable (CathodeAutopsy 
    src/core/application.cpp
    src/core/main.cpp
    src/core/paths.cpp

    src/data/grid.cpp 
    src/data/point_cloud.cpp
    src/data/primitives.cpp

    src/platform/input_manager.cpp
    src/platform/window.cpp
    
    src/rendering/camera.cpp
    src/rendering/renderer.cpp
    src/rendering/shader.cpp
    src/rendering/text_renderer.cpp
    src/rendering/texture.cpp
        
    src/ui/pause_screen.cpp
    src/ui/startup_screen.cpp
    src/ui/ui_geometry.cpp
    src/ui/running_screen.cpp
)

target_compile_options(CathodeAutopsy PRIVATE -Wall -Wextra -Wpedantic)

add_custom_command(TARGET CathodeAutopsy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:CathodeAutopsy>/shaders)

add_custom_command(TARGET CathodeAutopsy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:CathodeAutopsy>/shaders)

add_custom_command(TARGET CathodeAutopsy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/textures
        $<TARGET_FILE_DIR:CathodeAutopsy>/textures)

add_custom_command(TARGET CathodeAutopsy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/fonts
        $<TARGET_FILE_DIR:CathodeAutopsy>/fonts)

target_include_directories(CathodeAutopsy PRIVATE include)
target_include_directories(CathodeAutopsy SYSTEM PRIVATE
    external/prebuilt/glfw-3.4.bin.WIN64/include
    external/prebuilt/libE57Format-3.3.0-mingw-w64/include
    external/prebuilt/xerces-c-3.2.3-mingw-w64/include
)
target_link_directories(CathodeAutopsy PRIVATE 
    external/prebuilt/glfw-3.4.bin.WIN64/lib-mingw-w64
    external/prebuilt/libE57Format-3.3.0-mingw-w64/lib
    external/prebuilt/xerces-c-3.2.3-mingw-w64/lib
)
target_link_libraries(CathodeAutopsy PRIVATE
    glad
    glfw3
    opengl32
    glm::glm
    freetype
    tinyfiledialogs
    stb_image
    E57Format
    xerces-c
)

target_link_options(CathodeAutopsy PRIVATE 
    -static-libgcc -static-libstdc++ -static
)