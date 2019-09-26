#ifndef FOR_T_HPP_
#define FOR_T_HPP_

#include <isl_cpp_type.hpp>
#include <Set_Poly_t.hpp>

class Constraint_t;
class Poly_scope_t;

class Var_t : public Set_Poly_t{
	
	std::string name;
	
	Constraint_t* value;
	
	Set_Poly_t* inner;

	void fill_constraint(ISL::Set& set);
	
	void fill_constraint(ISL::Map& map);
	
	ISL::Set project_out_var(ISL::Set set);
	
	ISL::Map project_out_var(ISL::Map map);
	
	void check_inner(const std::string& msg);
	
public:

	std::string get_name() const{ return name; }
	
	template <typename T>
	Var_t(T _name, Constraint_t* _value, Set_Poly_t* _inner = nullptr) : 
		name(_name),
		value(_value),
		inner(_inner)
	{}
	
	Var_t* set_inner(Set_Poly_t* _inner);

	ISL::Set createSet(const V_Input_t& Vin);
	
	ISL::Set createSet(Poly_scope_t&);
	
	ISL::Set createSet(ISL::Set set);

	ISL::Map createMap(Poly_scope_t&, ISL::Set func_set);
	
	ISL::Map createMap(const V_Input_t& Vin, ISL::Set func_set);
	
	ISL::Map createMap(ISL::Map map);

	size_t dim_size();

	void scope(Poly_scope_t&);
	
	Var_t* clone() const;
	
	void print() const;
};


#endif /* FOR_T_HPP_ */
