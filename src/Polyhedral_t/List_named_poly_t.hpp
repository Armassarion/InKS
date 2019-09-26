#ifndef LIST_NAMED_POLY_T_HPP
#define LIST_NAMED_POLY_T_HPP

#include <vector>
#include <isl_cpp_type.hpp>

class Named_Poly_t;
class Set_Poly_t;
class List_var_t;
class V_Input_t;

class List_named_poly_t {
	
	std::vector<Set_Poly_t*> polys;
	
public:

	List_named_poly_t(Named_Poly_t* np);
	
	List_named_poly_t* push(Named_Poly_t* np);
		
	ISL::USet createSet(const V_Input_t& Vin, List_var_t& lvars);
	
};

#endif // LIST_NAMED_POLY_T_HPP
