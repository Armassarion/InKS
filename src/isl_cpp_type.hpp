#ifndef ISL_CPP_TYPE__
#define ISL_CPP_TYPE__

#include <iostream>

#include <isl/id.h>
#include <isl/ast.h>
#include <isl/val.h>
#include <isl/set.h>
#include <isl/map.h>
#include <isl/aff.h>
#include <isl/mat.h>
#include <isl/flow.h>
#include <isl/space.h>
#include <isl/printer.h>
#include <isl/schedule.h>
#include <isl/union_set.h>
#include <isl/union_map.h>
#include <isl/ast_build.h>
#include <isl/constraint.h>
#include <isl/local_space.h>
#include <isl/schedule_node.h>

namespace ISL{
	using Id = isl_id*;
	using Val = isl_val*;
	using Set = isl_set*;
	using Map = isl_map*;
	using Aff = isl_aff*;
	using Mat = isl_mat*;
	using Space = isl_space*;
	using PWAff = isl_pw_aff*;
	using BSet = isl_basic_set*;
	using BMap = isl_basic_map*;
	using USet = isl_union_set*;
	using UMap = isl_union_map*;
	using Sched = isl_schedule*;
	using M_Aff = isl_multi_aff*;
	using Printer = isl_printer*;
	using Flow = isl_union_flow*;
	using Ast_node = isl_ast_node*;
	using Ast_expr = isl_ast_expr*;
	using LSpace = isl_local_space*;
	using Ast_build = isl_ast_build*;
	using M_PWAff = isl_multi_pw_aff*;
	using PWM_Aff = isl_pw_multi_aff*;
	using Constraint = isl_constraint*;
	using Sched_node = isl_schedule_node*;
	using AccessInfo = isl_union_access_info*;
	using Ast_POption = isl_ast_print_options*;	
	
	using L_Id = isl_id_list*;
	using L_Map = isl_map_list*;
	using L_Set = isl_set_list*;
	using L_BSet = isl_basic_set_list*;
	using L_BMap = isl_basic_map_list*;
	using L_Constraint = isl_constraint_list*;
	
	/****STR ISL*/
	#define STR_ISL(type)															\
		inline const char* str(isl_##type *object){									\
			return (object == NULL) ? "[!NULL!]" : (isl_##type##_to_str(object));	\
		}																			\
	
	STR_ISL(pw_multi_aff)
	STR_ISL(multi_pw_aff)
	STR_ISL(multi_aff)
	STR_ISL(union_map)
	STR_ISL(union_set)
	STR_ISL(basic_set)
	STR_ISL(basic_map)
	STR_ISL(ast_node)
	STR_ISL(ast_expr)
	STR_ISL(pw_aff)
	STR_ISL(space)
	STR_ISL(set)
	STR_ISL(map)
	STR_ISL(aff)
	STR_ISL(val)
	
	/****Print ISL*/
	template <typename T>
	void print(T object){
		std::cout << str(object) << std::endl << std::endl;
	}	
	template <typename T>
	void print(T object, const std::string& msg_1){
		std::cout << msg_1 << " " << str(object) << std::endl << std::endl;
	}
	template <typename T>
	void print(T object, const std::string& msg_1, const std::string& msg_2){
		std::cout << msg_1 << " " << str(object) << std::endl << msg_2 << std::endl << std::endl;
	}
	
	/****Copy ISL*/
	#define COPY_ISL(type) 								\
		inline isl_##type *copy(isl_##type *object){	\
			return isl_##type##_copy(object);			\
		}												\
		
	COPY_ISL(pw_multi_aff)
	COPY_ISL(multi_pw_aff)
	COPY_ISL(local_space)
	COPY_ISL(constraint)
	COPY_ISL(multi_aff)
	COPY_ISL(union_map)
	COPY_ISL(union_set)
	COPY_ISL(basic_set)
	COPY_ISL(basic_map)
	COPY_ISL(pw_aff)
	COPY_ISL(space)		
	COPY_ISL(map)
	COPY_ISL(set)
	COPY_ISL(aff)
	COPY_ISL(id)
	
	/****Free ISL*/
	#define FREE_ISL(type) 						\
		inline void free(isl_##type *&object){	\
			isl_##type##_free(object);			\
			object = nullptr;					\
		}										\
	
	FREE_ISL(basic_set_list)
	FREE_ISL(basic_map_list)
	FREE_ISL(pw_multi_aff)
	FREE_ISL(multi_pw_aff)
	FREE_ISL(local_space)
	FREE_ISL(constraint)
	FREE_ISL(multi_aff)
	FREE_ISL(union_map)
	FREE_ISL(union_set)
	FREE_ISL(basic_set)
	FREE_ISL(basic_map)
	FREE_ISL(set_list)
	FREE_ISL(map_list)
	FREE_ISL(pw_aff)
	FREE_ISL(space)		
	FREE_ISL(map)
	FREE_ISL(set)
	FREE_ISL(aff)
	FREE_ISL(id)
	
}

#endif