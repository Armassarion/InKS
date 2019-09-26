#ifndef INKS_V_FUNCTION_APPLICATION_T_HPP__
#define INKS_V_FUNCTION_APPLICATION_T_HPP__

#include <vector>
#include <string>

#include <Function_application_t.hpp>
#include <isl_cpp_type.hpp>

class Fill_argument_res;
class Function_Map;
class List_var_t;
class Array_Map;
class V_Input_t;

struct V_Function_application_t{
	
	using Fa_iterator = std::vector<Function_application_t*>::iterator;
	
	std::vector<Function_application_t*> vfa;
	
	V_Function_application_t();
	
	V_Function_application_t(Function_application_t* _fa);
	
	void scope(Function_Map& Functions, Array_Map& Arrays);

	V_Function_application_t* push(Function_application_t* _fa);

	Function_application_t* get(size_t i);

	ISL::USet createApplySet(const V_Input_t& Vin, List_var_t& vars);

	ISL::UMap get_InRelation(Function_application_t::Instance_name_mode name_mode);

	ISL::UMap get_OutRelation(Function_application_t::Instance_name_mode name_mode);
	
	size_t size() const;

	void init();
			
	Function_application_t* find_from_func_name(std::string);
	
	Function_application_t* find_from_instance_name(std::string);
	
	Fill_argument_res fill_argument(std::string);

	size_t get_array_index(std::string array_name);
	
	Function_application_t* func_from_instance(std::string instance_name);
};

#endif
