#ifndef MULT_T_HPP_
#define MULT_T_HPP_

#include <isl_cpp_type.hpp>
#include <Expr_t.hpp>

class Poly_scope_t;

struct Mult_t : Expr_t{
	Expr_t* v;
	int multiplier;

	Mult_t(int _multiplier, Expr_t* _v) : 
		v(_v),
		multiplier(_multiplier)
	{}
	
	Mult_t(const char* _multiplier, Expr_t* _v) : 
		Mult_t(atoi(_multiplier), _v)
	{}

	int fill_constraint(ISL::Constraint& c, isl_dim_type type, int coef){
		return v->fill_constraint(c, type, coef*multiplier);
	}

	Expr_t* scope(Poly_scope_t& scope_object){
		v = v->scope(scope_object);
		return this;
	}
	
	Expr_t* clone() const{
		return new Mult_t(multiplier, v->clone());
	}
	
	void print() const {
		std::cout << multiplier << " * (";
		v->print();
		std::cout << ")";
	}
};


#endif /* MULT_T_HPP_ */
