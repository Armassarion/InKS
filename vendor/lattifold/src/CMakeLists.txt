set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(lattifold_src
	options.c
	isl_tools.c
	lattifold.c
	starshape.c
	heuristic_1.c
	heuristic_2.c
	heuristic_3.c
	partial_basis.c
	successive_modulo.c
)
add_library(lattifold ${lattifold_src})
set_property(TARGET lattifold PROPERTY C_STANDARD 99)
target_link_libraries(lattifold PUBLIC isl)
target_include_directories(lattifold PUBLIC ../include)
