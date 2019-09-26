#ifndef EQUALITY_T_HPP_
#define EQUALITY_T_HPP_

#include <vector>

#include <isl_cpp_type.hpp>
#include <Constraint_t.hpp>

class Poly_scope_t;
class Expr_t;

struct Equality_t : virtual public Constraint_t{
	Expr_t* eq;

	Equality_t(Expr_t* _eq);
		
	ISL::Constraint constraint_isl_alloc() const;

	virtual void create_constraint(std::vector<ISL::Constraint>& v, int dim, TYPE_ISL type);

	void scope(Poly_scope_t& scope_object);
	
	Constraint_t* clone() const;
	
	void print() const;
};

#endif /* EQUALITY_T_HPP_ */
