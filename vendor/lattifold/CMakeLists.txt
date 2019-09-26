project(Lattifold_lib C)
cmake_minimum_required(VERSION 2.8)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(ISL REQUIRED)
add_subdirectory(src/)
