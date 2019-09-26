#ifndef FUNCTION_T_HPP_
#define FUNCTION_T_HPP_

#include <string>

#include <V_Function_application_t.hpp>
#include <isl_cpp_type.hpp>

class Function_application_t;
class V_Dependence_t;
class Dependence_t;
class List_var_t;
class V_Input_t;
class List_t;

struct Fill_argument_res{
	std::string tmpl, arr_arg, struct_arg;
};

struct Function_t{
	
	enum COLUMN {SEMI_COLUMN, NO_SEMI_COLUMN};
	
	int id;
	
	std::string name;
	
	List_t* dimension;
	
	V_Dependence_t* vio;
	
	V_Function_application_t vfa;
	
	std::string code;
	
	std::string prototype;

	Function_t();
	
	Function_t(char* _name, List_t* _dimension, V_Dependence_t* _vio, int _id, char* _code);

	Function_t(std::string _name, List_t* _dimension, V_Dependence_t* _vio, int _id, std::string _code);

	size_t dim_size();

	size_t nb_array();

	size_t nb_param();

	void add_application(Function_application_t* _fa);

	ISL::Map getIn(size_t i);

	ISL::Map getOut(size_t i);
	
	ISL::UMap getIn();
	
	ISL::UMap getOut();
	
	ISL::UMap getIn_Applied();
	
	ISL::UMap getOut_Applied();

	void init(V_Input_t&, List_var_t& vars);

	int get_id();
	
	ISL::Set name_dimension_isl_set(ISL::Set&);
	
	ISL::Set name_isl_set(ISL::Set&);

	ISL::Set create_func_set(V_Input_t& Vin);
	
	Fill_argument_res fill_argument();
	
	std::string fill_template();

	std::string fill_arr_arg();
	
	std::string get_arr_arg();
	
	std::string get_arr_arg(int i);
			
	std::string get_prototype(COLUMN) const;
	
	std::string get_code() const;
	
	Dependence_t* get_dependence_from(std::string array_name);
	
	bool is_self_dependent(size_t);
};

#endif /* FUNCTION_T_HPP_ */
