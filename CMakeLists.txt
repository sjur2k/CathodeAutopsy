cmake_minimum_required (VERSION 3.10)

project (CathodeAutopsy)

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

add_library(glad STATIC src/glad/glad.c)

target_include_directories(glad SYSTEM PUBLIC include)

add_library(tinyfiledialogs STATIC
    src/tinyfiledialogs/tinyfiledialogs.c
)

target_include_directories(tinyfiledialogs SYSTEM PUBLIC include)
target_link_libraries(tinyfiledialogs PUBLIC comdlg32 ole32)

add_executable (CathodeAutopsy 
    src/main.cpp
    src/grid.cpp 
    src/camera.cpp
    src/shader.cpp
    src/renderer.cpp
    src/window.cpp
    src/input_manager.cpp
    src/application.cpp
    src/mesh.cpp
    src/text_renderer.cpp
    src/stb_image/stb_image.cpp
    src/texture.cpp
    src/paths.cpp
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
    lib/glfw-3.4.bin.WIN64/include
)
target_link_directories(CathodeAutopsy PRIVATE 
    lib/glfw-3.4.bin.WIN64/lib-mingw-w64
)
target_link_libraries(CathodeAutopsy PRIVATE
    glad
    glfw3
    opengl32
    glm::glm
    freetype
    tinyfiledialogs
)

target_link_options(CathodeAutopsy PRIVATE -static-libgcc -static-libstdc++ -static)