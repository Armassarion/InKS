#ifndef EXPR_T_HPP_
#define EXPR_T_HPP_

#include <Poly_Exception_t.hpp>
#include <isl_cpp_type.hpp>

class Poly_scope_t;

struct Expr_t{

	virtual int fill_constraint(ISL::Constraint& c, isl_dim_type type, int coef) = 0;
	virtual Expr_t* scope(Poly_scope_t& scope_object){
		return this;
	}
	virtual Expr_t* clone() const = 0;
	virtual void print() const = 0;
};


#endif /* EXPR_T_HPP_ */
