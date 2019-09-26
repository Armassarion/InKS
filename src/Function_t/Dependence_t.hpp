#ifndef DEPENDENCET_H_
#define DEPENDENCET_H_

#include <isl_cpp_type.hpp>

#include <exception>
#include <string>

class Poly_scope_t;
class List_var_t;
class Set_Poly_t;

struct Dependence_exception_t : public std::exception{
	std::string name;
	
	std::string dependence_type;
	
	Dependence_exception_t(std::string _n, std::string _dt) : name(_n), dependence_type(_dt){}
	
	virtual const char* what() const noexcept{
		std::string r = dependence_type + " dependence on " + name + " failed.";
		return r.c_str();
	}
};

struct Dependence_t
{	
	Set_Poly_t* set_in;
	
	Set_Poly_t* set_out;
	
	int arr_dimsize;

	std::string arr_name;
	
	std::string arr_type;
	
	ISL::Map Rin;
	
	ISL::Map Rout;

	Dependence_t(Set_Poly_t* _in, Set_Poly_t* _out);
	
	Dependence_t();
	
	Dependence_t(std::string);

	Dependence_t* add_arr_info(const char* _arr_name, const char* _arr_type);

	void initDep(Poly_scope_t scope_object, List_var_t& vars, size_t dim_size, isl_dim_type type);

	ISL::Map createDepMap(Poly_scope_t&, List_var_t& vars, Set_Poly_t* set);

	ISL::Map getIn();

	ISL::Map getOut();

	std::string get_arr_name();

	bool has_IO();
	
	void check_dim_size(size_t);
};

#endif /* DEPENDENCET_H_ */
