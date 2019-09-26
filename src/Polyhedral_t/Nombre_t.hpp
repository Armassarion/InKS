#ifndef NOMBRE_T_HPP_
#define NOMBRE_T_HPP_

#include <iostream>

#include <const_globals.hpp>
#include <isl_cpp_type.hpp>
#include <Expr_t.hpp>

struct Nombre_t : Expr_t
{
	int n;

	Nombre_t(int _n) : 
		n(_n)
	{}
	
	Nombre_t(const char* _n) : 
		Nombre_t(atoi(_n))
	{}

	int fill_constraint(ISL::Constraint& c, isl_dim_type type, int coef){
		ISL::Val v = isl_constraint_get_constant_val(c);
		v = isl_val_add(v, isl_val_int_from_si(ctx, n * coef));
		c = isl_constraint_set_constant_val(c, v);
		
		return -1;
	}
	
	Expr_t* clone() const{
		return new Nombre_t(n);
	}
	
	void print() const {
		std::cout << n;
	}
};

#endif /* NOMBRE_T_HPP_ */