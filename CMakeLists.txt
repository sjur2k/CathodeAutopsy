cmake_minimum_required (VERSION 3.10)

project (CathodeAutopsy)
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable (CathodeAutopsy src/main.cpp src/grid.cpp)
