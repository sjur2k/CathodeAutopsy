cmake_minimum_required (VERSION 3.10)

project (CathodeAutopsy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_compile_options(-Wall -Wextra -Wpedantic)

find_package(glm CONFIG REQUIRED)

add_executable (CathodeAutopsy 
    src/main.cpp
    src/grid.cpp 
    src/camera.cpp
    src/shader.cpp
    src/renderer.cpp
    src/window.cpp
    src/input_manager.cpp
    src/application.cpp
    src/glad/glad.c
)

target_include_directories(CathodeAutopsy PRIVATE 
    ${CMAKE_SOURCE_DIR}/lib/glfw-3.4.bin.WIN64/include
    ${CMAKE_SOURCE_DIR}/include
)
target_link_directories(CathodeAutopsy PRIVATE 
    ${CMAKE_SOURCE_DIR}/lib/glfw-3.4.bin.WIN64/lib-mingw-w64
)
target_link_libraries(CathodeAutopsy PRIVATE
    glfw3
    opengl32
    glm::glm
)