#ifndef INEQUALITY_T_HPP_
#define INEQUALITY_T_HPP_

#include <vector>

#include <isl_cpp_type.hpp>
#include <Constraint_t.hpp>

class Poly_scope_t;
class Expr_t;

struct Inequality_t : virtual public Constraint_t
{
	Expr_t* iq1;
	
	Expr_t* iq2;
	
	bool supremum;

	Inequality_t(Expr_t* _iq1, Expr_t* _iq2);
	
	ISL::Constraint constraint_isl_alloc() const;

	void create_constraint(std::vector<ISL::Constraint>& v, int dim, TYPE_ISL type);

	void scope(Poly_scope_t& scope_object);
	
	Constraint_t* unset_supremum();
	
	Constraint_t* clone() const;
	
	void print() const;
};

#endif /* INEQUALITY_T_HPP_ */
