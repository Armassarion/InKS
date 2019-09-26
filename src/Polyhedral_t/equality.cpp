#include <iostream>
#include <vector>

#include <Poly_scope_t.hpp>
#include <Equality_t.hpp>
#include <isl_utils.hpp>
#include <Expr_t.hpp>

using std::vector;

Equality_t::Equality_t(Expr_t* _eq) : 
	Constraint_t(),
	eq(_eq)
{}

ISL::Constraint Equality_t::constraint_isl_alloc() const{
	if(lspace == nullptr){
		std::cerr << "*** Error : Cannot alloc constraint without local space\n";
		exit(EXIT_FAILURE);
	}
	return isl_constraint_alloc_equality(ISL::copy(lspace));
}

void Equality_t::create_constraint(vector<ISL::Constraint>& v, int dim, TYPE_ISL type){
	if(eq != nullptr){
		ISL::Constraint c = constraint_isl_alloc();
		eq->fill_constraint(c, get_reverse_type_isl(type), -1);
		c = isl_constraint_set_coefficient_si(c, get_type_isl(type), dim, 1);
		v.push_back(c);
	}
}

void Equality_t::scope(Poly_scope_t& scope_object){
	if(eq != nullptr) eq = eq->scope(scope_object);
}

Constraint_t* Equality_t::clone() const{
	return new Equality_t(eq->clone());
}

void Equality_t::print() const{
	if(eq != nullptr)
		eq->print();
}