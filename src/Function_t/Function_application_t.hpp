#ifndef FUNCTIONAPPLICATIONT_H
#define FUNCTIONAPPLICATIONT_H

#include <string>
#include <vector>
#include <map>

#include <isl_cpp_type.hpp>

class Fill_argument_res;
class Function_Map;
class Set_Poly_t;
class Function_t;
class List_var_t;
class Array_Map;
class V_Input_t;
class List_t;
class Array;

struct Function_application_t{
	
private:

	static std::map<std::string, int> appl_map_id;
	
	uint my_id;
	
	void check_constructor();
	
	std::string ref_name;

	Set_Poly_t* set_dom;
	
	List_t* array_list;
	
public:

	enum Instance_name_mode {INSTANCE, KERNEL};
	
	ISL::UMap set_relation_name(ISL::Map, const char*, Instance_name_mode);
	
/*After scope*/
	
	Function_t* Func_ref;
	
	std::string instance_name;
	
	std::vector<Array*> Param_arrays;
		
	ISL::Set set;
	
	Function_application_t(std::string _ref_name, Set_Poly_t* _set, List_t* _param_arrays);
	
	void scope(Function_Map& Functions, Array_Map& Arrays);

	ISL::Set createIslSet(const V_Input_t& Vin, List_var_t& vars);
	
	ISL::UMap createInRelation(Instance_name_mode);
	
	ISL::UMap createOutRelation(Instance_name_mode);

	std::string instance() const;

	const char* arr_name(int i);
	
	bool refer_to(std::string);
	
	Function_t* get_ref();
	
	ISL::Set get_isl_set();
	
	ISL::Set get_named_isl_set(V_Input_t&);
	
	void name_dimension_isl_set(ISL::Set&);
	
	void name_isl_set(ISL::Set&);
	
	size_t nb_array() const;
		
	std::string get_arr_arg() const;
	
	std::string get_arr_arg(int i) const;
	
	Fill_argument_res fill_argument();
	
	size_t get_array_index(std::string array_name);
	
};

#endif
