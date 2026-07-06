cmake_minimum_required (VERSION 3.10)

project (CathodeAutopsy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_compile_options(-Wall -Wextra -Wpedantic)

# CHANGE THIS:  
set(GLFW_DIR "C:/sjur2k/lib/glfw-3.4.bin.WIN64") #Hardcoded path to glfw directory, since Hydro's firewall would not allow building from source.
find_package(glm CONFIG REQUIRED)

add_executable (CathodeAutopsy 
    src/main.cpp
    src/grid.cpp 
    src/camera.cpp
    src/shader.cpp
    src/glad.c
)

target_include_directories(CathodeAutopsy PRIVATE 
    ${GLFW_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
)
target_link_directories(CathodeAutopsy PRIVATE 
    ${GLFW_DIR}/lib-mingw-w64
)
target_link_libraries(CathodeAutopsy PRIVATE
    glfw3
    opengl32
    gdi32
    glm::glm
)