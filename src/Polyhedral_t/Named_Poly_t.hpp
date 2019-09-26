#ifndef NAMED_UNION_POLY_T_HPP
#define NAMED_UNION_POLY_T_HPP

#include <vector>

#include <isl_cpp_type.hpp>
#include <Set_Poly_t.hpp>

class Poly_scope_t;
class V_Input_t;

class Named_Poly_t : virtual public Set_Poly_t{
	
	std::string name;
	
	Set_Poly_t* poly;
	
public:

	Named_Poly_t();
	
	Named_Poly_t(const char* _n, Set_Poly_t* _p);
	
	Named_Poly_t(const char* _n);

	size_t dim_size();
	
	ISL::Set createSet(const V_Input_t& Vin);
	
	ISL::Set createSet(Poly_scope_t& scope_object);
	
	ISL::Set createSet(ISL::Set set);

	ISL::Map createMap(Poly_scope_t& scope_object, ISL::Set func_set);
	
	ISL::Map createMap(const V_Input_t& Vin, ISL::Set func_set);
	
	ISL::Map createMap(ISL::Map map);

	void scope(Poly_scope_t& scope_object);
	
	Set_Poly_t* clone() const;
	
	void print() const;

};

#endif // NAMED_UNION_POLY_T_HPP
