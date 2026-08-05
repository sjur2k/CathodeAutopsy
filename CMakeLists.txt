cmake_minimum_required (VERSION 3.10)

project (CloudWalk3D)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.5.1
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    XercesC
    GIT_REPOSITORY https://github.com/apache/xerces-c.git
    GIT_TAG v3.2.3
    OVERRIDE_FIND_PACKAGE
    EXCLUDE_FROM_ALL
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
FetchContent_MakeAvailable(XercesC)
if(NOT TARGET XercesC::XercesC)
    add_library(XercesC::XercesC ALIAS xerces-c)
endif()
target_include_directories(xerces-c PUBLIC
    $<BUILD_INTERFACE:${xercesc_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${xercesc_BINARY_DIR}/src>
)

FetchContent_Declare(
    libE57Format
    GIT_REPOSITORY https://github.com/asmaloney/libE57Format.git
    GIT_TAG c854ecc9b9c147ec3deaaa01153e7d95e48f2259 # Includes my O_BINARY fix
)
set(E57_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(E57_RELEASE_LTO OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libE57Format)

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

add_library(glad STATIC external/glad/glad.c)
target_include_directories(glad SYSTEM PUBLIC external/glad)

add_library(tinyfiledialogs STATIC external/tinyfiledialogs/tinyfiledialogs.c)
target_include_directories(tinyfiledialogs SYSTEM PUBLIC external/tinyfiledialogs)
target_link_libraries(tinyfiledialogs PUBLIC comdlg32 ole32)

add_library(stb_image STATIC external/stb_image/stb_image.cpp)
target_include_directories(stb_image SYSTEM PUBLIC external/stb_image)

add_executable (CloudWalk3D 
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

#target_compile_options(CloudWalk3D PRIVATE -Wall -Wextra -Wpedantic)

add_custom_command(TARGET CloudWalk3D POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:CloudWalk3D>/shaders)

add_custom_command(TARGET CloudWalk3D POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/textures
        $<TARGET_FILE_DIR:CloudWalk3D>/textures)

add_custom_command(TARGET CloudWalk3D POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/fonts
        $<TARGET_FILE_DIR:CloudWalk3D>/fonts)

target_include_directories(CloudWalk3D PRIVATE include)

target_link_libraries(CloudWalk3D PRIVATE
    glfw
    glm::glm
    E57Format
    xerces-c
    freetype
    glad
    stb_image
    tinyfiledialogs  
    opengl32
)

target_link_options(CloudWalk3D PRIVATE 
    -static-libgcc -static-libstdc++ -static
)