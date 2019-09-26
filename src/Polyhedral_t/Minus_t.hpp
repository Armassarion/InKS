#ifndef MINUS_T_HPP_
#define MINUS_T_HPP_

#include <isl_cpp_type.hpp>
#include <Expr_t.hpp>

class Poly_scope_t;

struct Minus_t : Expr_t{
	Expr_t* v1;
	Expr_t* v2;

	Minus_t(Expr_t* _v1, Expr_t* _v2):v1(_v1), v2(_v2){}

	int fill_constraint(ISL::Constraint& c, isl_dim_type type, int coef){
		int r1 = v1->fill_constraint(c, type, coef);
		int r2 = v2->fill_constraint(c, type, -coef);
		
		return r1 == -1 ? r2 : r1;
	}

	Expr_t* scope(Poly_scope_t& scope_object){
		v1 = v1->scope(scope_object);
		v2 = v2->scope(scope_object);
		return this;
	}
	
	Expr_t* clone() const{
		return new Minus_t(v1->clone(), v2->clone());
	}
	
	void print() const {
		v1->print();
		std::cout << " - ";
		v2->print();
	}
};


#endif /* MINUS_T_HPP_ */
