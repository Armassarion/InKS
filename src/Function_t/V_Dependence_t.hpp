#ifndef V_DEPENDENCE_T_HPP_
#define V_DEPENDENCE_T_HPP_

#include <string>
#include <vector>

#include <isl_cpp_type.hpp>
#include <Dependence_t.hpp>

class Poly_scope_t;
class List_var_t;
class V_Input_t;
class Array;

struct V_Dependence_exception_t : Dependence_exception_t{
	int n;
	
	V_Dependence_exception_t(Dependence_exception_t& _de, int _n);
};

struct V_Dependence_t{
	
	std::vector<Dependence_t*> vio;
	
	V_Dependence_t(Dependence_t* _d);

	V_Dependence_t* push(Dependence_t* _d);

	void initDep(Poly_scope_t& scope_object, List_var_t& vars, size_t dim_size, isl_dim_type type);

	Dependence_t* get(size_t i);

	ISL::Map getIn(size_t i);

	ISL::Map getOut(size_t i);
	
	ISL::UMap getIn(const char* in_name);

	ISL::UMap getOut(const char* in_name);
	
	size_t size();

	std::string get_arr_name(size_t i);
	
	std::string get_arr_name();
	
	void alter_arr_name(std::string new_name, std::string ex_name);

	void restore_arr_name(std::string new_name, std::string ex_name);
	
	std::vector<Dependence_t*>::iterator begin();

	std::vector<Dependence_t*>::iterator end();
	
	void check_arrays_dimsize(std::vector<Array*>& Arrays);

};

#endif /* V_DEPENDENCE_T_HPP_ */
