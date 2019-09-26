project(InKS CXX C)
cmake_minimum_required(VERSION 3.5.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

option(BUILD_INKS_EXAMPLES	"Build the examples provided with InKS" ON)
option(BUILD_INKS_TEST 		"Build the test main for InKS" OFF)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(ISL REQUIRED)
find_package(Boost 1.52.0 REQUIRED COMPONENTS program_options)

add_subdirectory(vendor/lattifold/)
add_subdirectory(src/)

if("${BUILD_INKS_EXAMPLES}")
	enable_testing()
	add_subdirectory(example)
endif()
