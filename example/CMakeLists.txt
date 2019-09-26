find_package(OpenMP REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPENMP_CXX_FLAGS}")

#heat_equation
add_custom_command(
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/heat_equation.kernels.hpp" "${CMAKE_CURRENT_BINARY_DIR}/heat_equation.sched.hpp"
		COMMAND inksC -a "${CMAKE_CURRENT_BINARY_DIR}/heat_equation.sched.hpp" -o "${CMAKE_CURRENT_BINARY_DIR}/heat_equation.kernels.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/heat_equation.ikf" -v
		MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/heat_equation.ikf"
)

#heat_equation_inf
add_custom_command(
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/heat_equation.kernels2.hpp" "${CMAKE_CURRENT_BINARY_DIR}/heat_equation.sched2.hpp"
		COMMAND inksC -a "${CMAKE_CURRENT_BINARY_DIR}/heat_equation.sched2.hpp" -o "${CMAKE_CURRENT_BINARY_DIR}/heat_equation.kernels2.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/heat_equation_inf.ikf" -v
		MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/heat_equation_inf.ikf"
)

message(${CMAKE_CURRENT_SOURCE_DIR})

#heat_equation_inkso1
add_custom_command(
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/heat_equation1.inkso.hpp"
		COMMAND inksC "${CMAKE_CURRENT_SOURCE_DIR}/heat_equation.ikf" -e "${CMAKE_CURRENT_SOURCE_DIR}/example_1.cpp" -f "${CMAKE_CURRENT_BINARY_DIR}/heat_equation1.inkso.hpp" -v
		MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/heat_equation.ikf"
)

#heat_equation_inkso2
add_custom_command(
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/heat_equation2.inkso.hpp"
		COMMAND inksC "${CMAKE_CURRENT_SOURCE_DIR}/heat_equation.ikf" -e "${CMAKE_CURRENT_SOURCE_DIR}/example_2.cpp" -f "${CMAKE_CURRENT_BINARY_DIR}/heat_equation2.inkso.hpp" -v
		MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/heat_equation.ikf"
)

add_executable(heat_equation_auto EXCLUDE_FROM_ALL "${CMAKE_CURRENT_BINARY_DIR}/heat_equation.kernels.hpp" main.cpp)
target_link_libraries(heat_equation_auto inks)
target_compile_definitions(heat_equation_auto PRIVATE USE_AUTO)

add_executable(heat_equation_auto_inf EXCLUDE_FROM_ALL "${CMAKE_CURRENT_BINARY_DIR}/heat_equation.kernels2.hpp" main.cpp)
target_link_libraries(heat_equation_auto_inf inks)
target_compile_definitions(heat_equation_auto_inf PRIVATE USE_AUTO_INF)

add_executable(heat_equation_inkso1 EXCLUDE_FROM_ALL "${CMAKE_CURRENT_BINARY_DIR}/heat_equation1.inkso.hpp" main.cpp)
target_link_libraries(heat_equation_inkso1 inks)
target_compile_definitions(heat_equation_inkso1 PRIVATE USE_INKSO1)

add_executable(heat_equation_inkso2 EXCLUDE_FROM_ALL "${CMAKE_CURRENT_BINARY_DIR}/heat_equation2.inkso.hpp" main.cpp)
target_link_libraries(heat_equation_inkso2 inks)
target_compile_definitions(heat_equation_inkso2 PRIVATE USE_INKSO2)

add_test(NAME heat_equation_auto_build COMMAND "${CMAKE_COMMAND}" --build . --target heat_equation_auto)
add_test(NAME heat_equation_auto_run COMMAND ${CMAKE_CURRENT_BINARY_DIR}/heat_equation_auto 10 10)
set_property(TEST heat_equation_auto_run PROPERTY DEPENDS heat_equation_auto_build)

add_test(NAME heat_equation_auto_inf_build COMMAND "${CMAKE_COMMAND}" --build . --target heat_equation_auto_inf)
add_test(NAME heat_equation_auto_inf_run COMMAND ${CMAKE_CURRENT_BINARY_DIR}/heat_equation_auto_inf 10 10)
set_property(TEST heat_equation_auto_inf_run PROPERTY DEPENDS heat_equation_auto_inf_build)

add_test(NAME heat_equation_inkso1_build COMMAND "${CMAKE_COMMAND}" --build . --target heat_equation_inkso1)
add_test(NAME heat_equation_inkso1_run COMMAND ${CMAKE_CURRENT_BINARY_DIR}/heat_equation_inkso1 10 10)
set_property(TEST heat_equation_inkso1_run PROPERTY DEPENDS heat_equation_inkso1_build)

add_test(NAME heat_equation_inkso2_build COMMAND "${CMAKE_COMMAND}" --build . --target heat_equation_inkso2)
add_test(NAME heat_equation_inkso2_run COMMAND ${CMAKE_CURRENT_BINARY_DIR}/heat_equation_inkso2 10 10)
set_property(TEST heat_equation_inkso2_run PROPERTY DEPENDS heat_equation_inkso2_build)

add_executable(heat_equation_opt EXCLUDE_FROM_ALL "${CMAKE_CURRENT_BINARY_DIR}/heat_equation.kernels.hpp" "${CMAKE_CURRENT_BINARY_DIR}/heat_equation.sched.hpp" main.cpp)
target_link_libraries(heat_equation_opt inks)
add_test(NAME heat_equation_opt_build COMMAND "${CMAKE_COMMAND}" --build . --target heat_equation_opt)
add_test(NAME heat_equation_opt_run COMMAND ${CMAKE_CURRENT_BINARY_DIR}/heat_equation_opt 10 10)
set_property(TEST heat_equation_opt_run PROPERTY DEPENDS heat_equation_opt_build)

add_test(NAME Heat_equation_diff COMMAND diff auto.dat opt.dat)
add_test(NAME Heat_equation_inf_diff COMMAND diff auto_inf.dat opt.dat)
add_test(NAME Heat_equation_inkso1_diff COMMAND diff auto_inf.dat inkso1.dat)
add_test(NAME Heat_equation_inkso2_diff COMMAND diff auto_inf.dat inkso2.dat)
