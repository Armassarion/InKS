#ifndef UNION_POLY_T_HPP_
#define UNION_POLY_T_HPP_

#include <vector>

#include <isl_cpp_type.hpp>
#include <Set_Poly_t.hpp>

class Poly_scope_t;
class V_Input_t;

struct Union_Poly_t : Set_Poly_t
{
	std::vector<Set_Poly_t*> polys;

	Union_Poly_t();
	
	Union_Poly_t(Set_Poly_t* p);

	Union_Poly_t* push(Set_Poly_t* p);

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


#endif /* UNION_POLY_T_HPP_ */
