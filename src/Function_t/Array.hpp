#ifndef ARRAY_HPP_
#define ARRAY_HPP_

#include <string>
#include <vector>
#include <list>

#include <isl_cpp_type.hpp>

class Dependence_t;
class List_var_t;
class V_Input_t;

struct Array{
	
	static int current_id;
	
	static const std::string COPY_NAME;
	
	int array_id;
	
//Parsing Attribute
	
	std::string type;
	
	std::string name;
	
	size_t dim_size;
	
	Dependence_t* io;
	
	bool used;
	
	//set representing the maximum value in each dimension, according to conditions
	ISL::Set isl_set_max_value;
	
	//pw_aff representing the maximum value in each dimension, according to conditions
	std::vector<ISL::PWAff> isl_max_value;
	
	//Condition of non existence in isl_form = ( (c10 || c11 || ... ) && (...) ) with c1x = condition on xth dimensions
	ISL::Set isl_wcondition;
	
	/*Method*/	
	Array(){};
	
	Array(const char* _type, const char* _name, const char* _dim_size, Dependence_t* _io);
	
	Array(const char* _type, const char* _name, const char* _dim_size);
	
	void initDep(V_Input_t& Vin, List_var_t& vars);
	
	bool is_scalar() const;

	const char* c_str();

	ISL::Map getOut();
	
	ISL::Map getIn();
	
	//set representing the logical array
	ISL::Set logical_set() const;
	
	ISL::Set max_logical_set() const;

	bool is_IO() const;
	
	void compute_valid_parameter(const V_Input_t& Vin, ISL::Set);
	
	void compute_modulo_condition(ISL::Set valid_param);
	
	void intersect_valid_parameter(ISL::Set valid_parameters);
};

#endif /* ARRAY_HPP_ */
