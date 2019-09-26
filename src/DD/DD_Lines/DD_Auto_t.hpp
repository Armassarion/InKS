#ifndef DD_AUTO_T_HPP
#define DD_AUTO_T_HPP

#include <string>
#include <vector>
#include <map>

#include <isl_cpp_type.hpp>
#include <DD_Update_t.hpp>

class List_named_poly_t;
class DD_Gen_t;

class DD_Auto_t : virtual public DD_Update_t{

protected:

	virtual void print_kernel() const;
	
public:

	DD_Auto_t(int _lineno, List_named_poly_t* _up);
	
	void scope(DD_Scope_t& scope_object);
};

#endif // DD_AUTO_T_HPP
