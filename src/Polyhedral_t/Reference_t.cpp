#include <iostream>

#include <Poly_scope_t.hpp>
#include <Reference_t.hpp>
#include <isl_utils.hpp>

int Constante_t::fill_constraint(ISL::Constraint& c, isl_dim_type type, int coef){
	c = isl_constraint_set_coefficient_si(c, isl_dim_param, pos, coef);
	return -pos-1;
}

int Reference_t::fill_constraint(ISL::Constraint& c, isl_dim_type type, int coef){
	if(pos == -1) throw Ref_Exception_t(name);
	c = isl_constraint_set_coefficient_si(c, type, pos, coef);
	return pos;
}

Expr_t* Reference_t::scope(Poly_scope_t& scope_object){	
	//Must check ref first
	if( (pos=scope_object.pos_ref(name)) != -1){
		return this;
	}
	
	if( (pos=scope_object.pos_const(name)) != -1){
		return new Constante_t(name, pos);
	}
	
	throw Ref_Exception_t(name);
}

Expr_t* Reference_t::clone() const{
	return new Reference_t(strdup(name), pos);
}

void Reference_t::print() const{
	std::cout << name;
}

Expr_t* Constante_t::clone() const{
	return new Constante_t(strdup(name), pos);
}