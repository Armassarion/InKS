#include <List_constraints_t.hpp>

List_constraints_t::List_constraints_t()
{}

List_constraints_t::List_constraints_t(Constraint_t* _c){
	constraints.push_back(_c);
}

List_constraints_t* List_constraints_t::push(Constraint_t* _c){
	constraints.push_back(_c);
	return this;
}

void List_constraints_t::create_constraint(std::vector<ISL::Constraint>& v, int dim, TYPE_ISL type){
	for(auto& c : constraints)
		c->create_constraint(v, dim, type);
}

void List_constraints_t::scope(Poly_scope_t& scope_object){
	for(auto& c : constraints)
		c->scope(scope_object);
}

Constraint_t* List_constraints_t::clone() const{
	List_constraints_t* r = new List_constraints_t();
	for(auto& c : constraints)
		r->push(c->clone());
	return r;
}


void List_constraints_t::init_lspace(ISL::LSpace _lspace){
	for(auto& c : constraints)
		c->init_lspace(_lspace);
}

bool List_constraints_t::is_modulo(){
	for(auto& c : constraints){
		if(c->is_modulo())
			return true;
	}
	
	return false;
}

void List_constraints_t::print() const{
	for(auto& c : constraints)
		c->print();
}
