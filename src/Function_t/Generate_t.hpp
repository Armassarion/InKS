#ifndef GENERATE_T_HPP_
#define GENERATE_T_HPP_

#include <string>

#include <isl_cpp_type.hpp>

#include <List_var_t.hpp>
#include <Input_t.hpp>

class V_Function_application_t;
class Function_application_t;
class Function_Map;
class Array_Map;
class V_Input_t;
class List_t;

struct Generate_t{
	
	std::string name;
	
	V_Input_t Vin;
	
	Array_Map* Arrays;
	
	V_Function_application_t* Apply;
	
	std::string restrict;
	
	List_var_t vars;

	void init(const char* _name, List_t* _param, Array_Map* Arrays, V_Function_application_t* _vfa, const char* _restrict="");
	
	void scope(Function_Map& Functions);

	void add_vars(List_var_t* _vars);

	bool no_generate();

	void initDep();
	
	ISL::Set get_restrict() const;
	
	ISL::UMap createOutputMap();
	
	ISL::UMap createInputMap();
	
	ISL::USet createApplySet();

	ISL::UMap createInRelation();

	ISL::UMap createOutRelation();
	
	std::string get_params(bool=false);
	
	//Compute valid_parameters + max value in each dimension + associated condition
	ISL::Set array_isl_property(ISL::USet& SI, ISL::USet& SO);
	
	Function_application_t* search_in_domain(std::string& instance);
	
	Function_application_t* fa_from_instance_name(const std::string& instance_name);
	
	Function_application_t* find_from_func_name(const std::string&);
};

#endif /* GENERATE_T_HPP_ */
