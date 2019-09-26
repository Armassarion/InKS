#include <isl_utils.hpp>
#include <Modulo_t.hpp>
#include <Expr_t.hpp>

Modulo_t::Modulo_t(Constraint_t* _c, Expr_t* _e, int _modulo) :
	Constraint_t(),
	c(_c),
	min(_e),
	modulo(_modulo)
{
	if(c->is_modulo()){
		std::cerr << "***Error: Modulo on a modulo constraint isnt supported\n";
		exit(0);
	}
}

Modulo_t::Modulo_t(Constraint_t* _c, Expr_t* _e, const char* _modulo) :
	Modulo_t(_c, _e, atoi(_modulo))
{}
	
void Modulo_t::init_lspace(ISL::LSpace _lspace){
	Constraint_t::init_lspace(_lspace);
	c->init_lspace(_lspace);
}

ISL::Constraint Modulo_t::constraint_isl_alloc() const{
	if(lspace == nullptr){
		std::cerr << "*** Error : Cannont alloc constraint without local space\n";
		exit(EXIT_FAILURE);
	}
	return isl_constraint_alloc_equality(ISL::copy(lspace));
}

bool Modulo_t::is_modulo(){
	return true;
}
	
// dim = stride * E
//Example => "[m, M] -> {F[b] -> [b]: m <= b < M and (b-m)%2 = 0 }"
void Modulo_t::create_constraint(std::vector<ISL::Constraint>& v, int dim, TYPE_ISL type){
	c->create_constraint(v, dim, type);
	
	int exist_dim = isl_local_space_dim(lspace, get_type_isl(type)) - 1;
	ISL::Constraint c_mod = constraint_isl_alloc();
	
	min->fill_constraint(c_mod, get_type_isl(type), 1);
	c_mod = isl_constraint_set_coefficient_si(c_mod, get_type_isl(type), dim, -1);
	c_mod = isl_constraint_set_coefficient_si(c_mod, get_type_isl(type), exist_dim, modulo);
	v.push_back(c_mod);
	
}

void Modulo_t::scope(Poly_scope_t& scope_object){
	c->scope(scope_object);
	min = min->scope(scope_object);
}

Constraint_t* Modulo_t::clone() const{
	return new Modulo_t(c->clone(), min->clone(), modulo);
}

void Modulo_t::print() const {
	c->print();
	std::cout << ":" << modulo;
}
	
//--------------ModuloT_t 

ModuloT_t::ModuloT_t(Constraint_t* _c, int _modulo) : 
	Constraint_t(),
	c(_c),
	modulo(_modulo)
{
	if(c->is_modulo()){
		std::cerr << "***Error: Modulo on a modulo constraint isnt supported\n";
		exit(0);
	}
}
	
ModuloT_t::ModuloT_t(Constraint_t* _c, const char* _modulo) : 
	ModuloT_t(_c, atoi(_modulo))
{}
	
void ModuloT_t::init_lspace(ISL::LSpace _lspace){
	Constraint_t::init_lspace(_lspace);
	c->init_lspace(_lspace);
}

ISL::Constraint ModuloT_t::constraint_isl_alloc() const{
	if(lspace == nullptr){
		std::cerr << "*** Error : Cannont alloc constraint without local space\n";
		exit(EXIT_FAILURE);
	}
	return isl_constraint_alloc_equality(ISL::copy(lspace));
}

bool ModuloT_t::is_modulo(){
	return true;
}

void ModuloT_t::create_constraint(std::vector<ISL::Constraint>& v, int dim, TYPE_ISL type){
	c->create_constraint(v, dim, type);
	int exist_dim = isl_local_space_dim(lspace, get_type_isl(type)) - 1;
	
	for(ISL::Constraint& constraint : v){
		constraint = isl_constraint_set_coefficient_si(constraint, get_type_isl(type), dim, -1);
		constraint = isl_constraint_set_coefficient_si(constraint, get_type_isl(type), exist_dim, modulo);
	}
	/*
	ISL::Constraint cstr = isl_constraint_alloc_inequality(ISL::copy(lspace));
	cstr = isl_constraint_set_coefficient_si(cstr, get_type_isl(type), dim, 1);
	cstr = isl_constraint_set_constant_si(cstr, -1);
	v.push_back(cstr);	
	*/
}

void ModuloT_t::scope(Poly_scope_t& scope_object){
	c->scope(scope_object);
}

Constraint_t* ModuloT_t::clone() const{
	return new ModuloT_t(c->clone(), modulo);
}

void ModuloT_t::print() const {
	std::cout << "(";
	c->print();
	std::cout << ") %" << modulo;
}