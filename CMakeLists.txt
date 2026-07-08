cmake_minimum_required (VERSION 3.10)

project (CathodeAutopsy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glm CONFIG REQUIRED)
find_package(Freetype REQUIRED)

add_library(glad STATIC 
    ${CMAKE_SOURCE_DIR}/src/glad/glad.c)

target_include_directories(glad SYSTEM PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

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
)

target_compile_options(CathodeAutopsy PRIVATE -Wall -Wextra -Wpedantic)

add_custom_command(TARGET CathodeAutopsy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:CathodeAutopsy>/shaders)

target_include_directories(CathodeAutopsy PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(CathodeAutopsy SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/glfw-3.4.bin.WIN64/include
)
target_link_directories(CathodeAutopsy PRIVATE 
    ${CMAKE_SOURCE_DIR}/lib/glfw-3.4.bin.WIN64/lib-mingw-w64
)
target_link_libraries(CathodeAutopsy PRIVATE
    glad
    glfw3
    opengl32
    glm::glm
    Freetype::Freetype
)