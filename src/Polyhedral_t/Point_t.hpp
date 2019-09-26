#ifndef POINT_T_HPP_
#define POINT_T_HPP_

#include <vector>

#include <isl_cpp_type.hpp>
#include <Set_Poly_t.hpp>

class Constraint_t;
class Poly_scope_t;
class List_t;

class Point_t : public Set_Poly_t
{
	std::vector<Constraint_t*> pt;

public:

	Point_t();

	Point_t(List_t* l);
	
	Point_t(Constraint_t* c);
	
	Point_t* push(Constraint_t* e);

	Point_t* push(const char* rname);

	size_t dim_size();
	
	ISL::Set createSet(Poly_scope_t& scope_object);
	
	ISL::Set createSet(const V_Input_t& Vin);
	
	ISL::Set createSet(ISL::Set set);

	ISL::Map createMap(Poly_scope_t& scope_object, ISL::Set func_set);
	
	ISL::Map createMap(const V_Input_t& Vin, ISL::Set func_set);
	
	ISL::Map createMap(ISL::Map map);

	void scope(Poly_scope_t& scope_object);
	
	Set_Poly_t* clone() const;
	
	void print() const;
};

#endif /* POINT_T_HPP_ */
