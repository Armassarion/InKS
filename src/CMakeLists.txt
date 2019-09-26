set(CMAKE_INCLUDE_CURRENT_DIR ON)

#Semantics parser
if(${CMAKE_BUILD_TYPE} MATCHES DEBUG)
	message("Debug mode, add -Wall")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -g")
	bison_target(inksC_parser parser.y "${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cc" COMPILE_FLAGS "--warnings=all")
else()
	bison_target(inksC_parser parser.y "${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cc")
endif()
flex_target(inksC_scanner scanner.lex "${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc")
add_flex_bison_dependency(inksC_scanner inksC_parser)

#InKSoDD parser
bison_target(DD_parser DD/dd_parser.y "${CMAKE_CURRENT_BINARY_DIR}/dd_parser.tab.cc" COMPILE_FLAGS "--report=all")
flex_target(DD_scanner DD/dd_scanner.lex "${CMAKE_CURRENT_BINARY_DIR}/dd_lex.yy.cc")
add_flex_bison_dependency(DD_scanner DD_parser)
	
	
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG -g")

set(inksC_src

		Function_t/list.cpp
		Function_t/array.cpp
		Function_t/function.cpp
		Function_t/generate.cpp
		Function_t/array_map.cpp
		Function_t/dependence.cpp
		Function_t/function_map.cpp
		Function_t/v_dependence.cpp
		Function_t/function_application.cpp
		Function_t/v_function_application.cpp
		
		Implem/Codes/If_t.cpp
		Implem/Codes/For_t.cpp
		Implem/Codes/Exit_t.cpp
		Implem/Codes/Block_t.cpp
		Implem/Codes/Buffer_t.cpp
		Implem/Codes/Variable_t.cpp
		Implem/Codes/Mult_array_t.cpp
		Implem/Codes/Auto_buffer_t.cpp
		Implem/Codes/Buffer_class_t.cpp
		Implem/Codes/Delete_buffer_t.cpp
		Implem/Codes/Function_call_t.cpp
		Implem/Codes/OMP_Parallel_Region_t.cpp
		
		Implem/Declarations/Function_Decl_t.cpp
		Implem/Declarations/Auto_buffer_decl_t.cpp
		Implem/Declarations/Buffer_class_decl_t.cpp
		
		Implem/IO_t.cpp
		Implem/V_IO_t.cpp
		Implem/Input_t.cpp
		Implem/Implem_t.cpp
		Implem/V_Code_t.cpp
		Implem/include_t.cpp
		Implem/V_Declaration_t.cpp
		
		Polyhedral_t/var.cpp
		Polyhedral_t/point.cpp
		Polyhedral_t/equality.cpp
		Polyhedral_t/inequality.cpp
		Polyhedral_t/Modulo_t.cpp
		Polyhedral_t/List_var_t.cpp
		Polyhedral_t/union_poly.cpp
		Polyhedral_t/Reference_t.cpp
		Polyhedral_t/Named_Poly_t.cpp
		Polyhedral_t/List_named_poly_t.cpp
		Polyhedral_t/List_constraints_t.cpp
		
		DD/DD.cpp
		DD/DD_Gen_t.cpp
		DD/DD_Code_t.cpp
		DD/DD_Block_t.cpp
		DD/V_DD_Block_t.cpp

		DD/Scope/DD_Scope_t.cpp
		DD/Scope/Time_Event_t.cpp
		DD/Scope/Modification_t.cpp
		DD/Scope/V_Modification_t.cpp
		
		DD/DD_Lines/DD_For_t.cpp
		DD/DD_Lines/DD_Map_t.cpp
		DD/DD_Lines/DD_Mem_t.cpp
		DD/DD_Lines/DD_Copy_t.cpp
		DD/DD_Lines/DD_Auto_t.cpp
		DD/DD_Lines/DD_Decl_t.cpp
		DD/DD_Lines/DD_Unmap_t.cpp
		DD/DD_Lines/DD_Param_t.cpp
		DD/DD_Lines/DD_Update_t.cpp
		DD/DD_Lines/DDFor_utils/V_DDForNamedVar.cpp
		DD/DD_Lines/DDFor_utils/Options/OMP_For_option_t.cpp
		DD/DD_Lines/DDFor_utils/Options/OMP_TaskLoop_option_t.cpp
		DD/DD_Lines/DDFor_utils/Options/DDForOptions_t.cpp
		
		utils.cpp
		automatic.cpp
		isl_utils.cpp
		isl_processing.cpp
		
		#Parser
		${BISON_inksC_parser_OUTPUTS}
		${FLEX_inksC_scanner_OUTPUTS}
		${BISON_DD_parser_OUTPUTS}
		${FLEX_DD_scanner_OUTPUTS}
)
	
add_executable(inksC
		${inksC_src}
		main.cpp
)
target_link_libraries(inksC
		Boost::program_options
		lattifold
		#${FLEX_LIBRARIES}
		#${BISON_LIBRARIES}
)
target_include_directories(inksC PRIVATE
	Polyhedral_t
	Function_t
	DD
	DD/DD_Lines
	DD/Scope
	Implem
)

set_property(TARGET inksC PROPERTY CXX_STANDARD 11)

add_library(inks INTERFACE)
target_compile_features(inks INTERFACE cxx_variadic_templates)
target_include_directories(inks INTERFACE
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>"
)

install(TARGETS inksC inks EXPORT inks_export
		RUNTIME DESTINATION bin INCLUDES DESTINATION include)

#INKS TEST
if("${BUILD_INKS_TEST}")
	add_executable(inksTest
			${inksC_src}
			main_test.cpp
	)
	target_link_libraries(inksTest
			Boost::program_options
			lattifold
			#${FLEX_LIBRARIES}
			#${BISON_LIBRARIES}
	)
	target_include_directories(inksTest PRIVATE
		Polyhedral_t
		Function_t
		DD
		DD/DD_Lines
		DD/Scope
		Implem
	)

	set_property(TARGET inksTest PROPERTY CXX_STANDARD 11)
endif()
