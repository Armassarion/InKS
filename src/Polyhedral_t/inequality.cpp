#include <vector>

#include <const_globals.hpp>
#include <Inequality_t.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>
#include <Expr_t.hpp>

using std::vector;

Inequality_t::Inequality_t(Expr_t* _iq1, Expr_t* _iq2) : 
	Constraint_t(),
	iq1(_iq1), 
	iq2(_iq2), 
	supremum(true)
{}

ISL::Constraint Inequality_t::constraint_isl_alloc() const{
	if(lspace == nullptr){
		std::cerr << "*** Error : Cannont alloc constraint without local space\n";
		exit(EXIT_FAILURE);
	}
	return isl_constraint_alloc_inequality(ISL::copy(lspace));
}

void Inequality_t::create_constraint(vector<ISL::Constraint>& v, int dim, TYPE_ISL type){
	isl_dim_type n_type = get_reverse_type_isl(type);
	isl_dim_type dim_type = get_type_isl(type);
	ISL::Constraint c;
	
	if(iq1 != nullptr){
		c = constraint_isl_alloc();
		c = isl_constraint_set_coefficient_si(c, dim_type, dim, 1);
		iq1->fill_constraint(c, n_type, -1);
		v.push_back(c);
	}
	
	if(iq2 != nullptr){
		c = constraint_isl_alloc();
		c = isl_constraint_set_coefficient_si(c, dim_type, dim, -1);
		iq2->fill_constraint(c, n_type, 1);
		
		//handle the supremum
		if(supremum) c = isl_constraint_set_constant_val(c, isl_val_sub(isl_constraint_get_constant_val(c), isl_val_one(ctx)));
		v.push_back(c);
	}
}

void Inequality_t::scope(Poly_scope_t& scope_object){
	if(iq1 != nullptr) iq1 = iq1->scope(scope_object);
	if(iq2 != nullptr) iq2 = iq2->scope(scope_object);
}

Constraint_t* Inequality_t::unset_supremum(){
	supremum = false;
	return this;
}

Constraint_t* Inequality_t::clone() const{
	Inequality_t* in = new Inequality_t(iq1->clone(), iq2->clone());
	if(!supremum) in->unset_supremum();
	
	return in;
}

void Inequality_t::print() const{
	std::cout << " [";
	if(iq1 != nullptr){
		iq1->print();
		std::cout << " : ";
	}
	if(iq2 != nullptr){
		iq2->print();
	}
	std::cout << (supremum ? " [ " : " ] ");
}