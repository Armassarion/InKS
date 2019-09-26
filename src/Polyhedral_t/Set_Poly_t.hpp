#ifndef SET_POLY_T_HPP_
#define SET_POLY_T_HPP_

#include <string>

#include <isl_cpp_type.hpp>

class Poly_scope_t;
class V_Input_t;

struct Set_Poly_t{
	
	virtual ISL::Set createSet(const V_Input_t& Vin) = 0;
	virtual ISL::Set createSet(Poly_scope_t& scope_object) = 0;
	virtual ISL::Set createSet(ISL::Set set) = 0;
	
	virtual ISL::Map createMap(const V_Input_t& Vin, ISL::Set func_set) = 0;
	virtual ISL::Map createMap(Poly_scope_t& scope_object, ISL::Set func_set) = 0;
	virtual ISL::Map createMap(ISL::Map map) = 0;
	
	virtual Set_Poly_t* clone() const = 0;
	virtual void print() const = 0;
	virtual size_t dim_size() = 0;
	virtual void scope(Poly_scope_t& scope_object) = 0;
};


#endif /* SET_POLY_T_HPP_ */
