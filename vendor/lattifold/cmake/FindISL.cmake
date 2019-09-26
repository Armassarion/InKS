# Try to find the isl library

# ISL_FOUND       - System has isl lib

cmake_minimum_required(VERSION 3.5.3)

include(FindPackageHandleStandardArgs)
include(CheckPrototypeDefinition)

function(find_isl_library)
	if (NOT TARGET isl)
		find_library(ISL_LIBRARY NAMES isl)
		if("${ISL_LIBRARY}" MATCHES ".*\.a$")
			add_library(isl STATIC IMPORTED GLOBAL)
		else()
			add_library(isl SHARED IMPORTED GLOBAL)
		endif()
		set_property(TARGET isl PROPERTY IMPORTED_LOCATION "${ISL_LIBRARY}")
		find_path(ISL_INCLUDE_DIR NAMES isl/version.h)
		set_property(TARGET isl PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${ISL_INCLUDE_DIR}")
		mark_as_advanced(ISL_INCLUDE_DIR ISL_LIBRARY)
	endif()
endfunction()

find_isl_library()

find_package_handle_standard_args(ISL DEFAULT_MSG ISL_LIBRARY)

