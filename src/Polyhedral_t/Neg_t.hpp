#ifndef NEG_T_HPP_
#define NEG_T_HPP_

#include <isl_cpp_type.hpp>
#include <Expr_t.hpp>

class Poly_scope_t;

struct Neg_t : Expr_t{
	Expr_t* v;

	Neg_t(Expr_t* _v):v(_v){}

	int fill_constraint(ISL::Constraint& c, isl_dim_type type, int coef){
		return v->fill_constraint(c, type, -coef);
	}

	Expr_t* scope(Poly_scope_t& scope_object){
		v = v->scope(scope_object);
		return this;
	}
	
	Expr_t* clone() const{
		return new Neg_t(v->clone());
	}
	
	void print() const {
		std::cout << "-";
		v->print();
	}
};


#endif /* NEG_T_HPP_ */
