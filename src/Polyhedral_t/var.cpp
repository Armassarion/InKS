#include <iostream>
#include <cstring>
#include <vector>

#include <const_globals.hpp>
#include <Constraint_t.hpp>
#include <Poly_scope_t.hpp>
#include <isl_utils.hpp>
#include <Var_t.hpp>

using std::vector;
using std::cout;
using std::cerr;
using std::endl;
	
Var_t* Var_t::set_inner(Set_Poly_t* _inner){
	inner = _inner;
	return this;
}

void Var_t::fill_constraint(ISL::Set& set){
	int new_dim = isl_set_dim(set, isl_dim_param);
	vector<ISL::Constraint> cs;
	
	set = isl_set_add_dims(set, isl_dim_param, 1);
	set = isl_set_set_dim_id(set, isl_dim_param, new_dim, isl_id_alloc(ctx, name.c_str(), nullptr));
	
	value->init_lspace(lspace_from_set(set));
	value->create_constraint(cs, new_dim, Constraint_t::TYPE_ISL::Param);
	for(ISL::Constraint c : cs)
		set = isl_set_add_constraint(set, c);
}

void Var_t::fill_constraint(ISL::Map& map){
	int new_dim = isl_map_dim(map, isl_dim_param);
	vector<ISL::Constraint> cs;
	
	map = isl_map_add_dims(map, isl_dim_param, 1);
	map = isl_map_set_dim_id(map, isl_dim_param, new_dim, isl_id_alloc(ctx, name.c_str(), nullptr));
	
	value->init_lspace(lspace_from_map(map));
	value->create_constraint(cs, new_dim, Constraint_t::TYPE_ISL::Param);
	for(ISL::Constraint c : cs)
		map = isl_map_add_constraint(map, c);
}

ISL::Set Var_t::project_out_var(ISL::Set set){	
	return isl_set_project_out(set, isl_dim_param, isl_set_dim(set, isl_dim_param)-1, 1);
}

ISL::Map Var_t::project_out_var(ISL::Map map){
	return isl_map_project_out(map, isl_dim_param, isl_map_dim(map, isl_dim_param)-1, 1);
}

ISL::Set Var_t::createSet(const V_Input_t& Vin){
	Poly_scope_t polyscope(Vin);
	return this->createSet(polyscope);
}

ISL::Set Var_t::createSet(Poly_scope_t& scope_object){
	this->scope(scope_object);
	return this->createSet(isl_set_universe(scope_object.set_space(0)));
}

ISL::Set Var_t::createSet(ISL::Set set){
	this->check_inner("create a set");
	fill_constraint(set);
	set = inner->createSet(set);
	return project_out_var(set);
}

ISL::Map Var_t::createMap(const V_Input_t& Vin, ISL::Set func_set){
	Poly_scope_t polyscope(Vin, func_set);
	return this->createMap(polyscope, func_set);
}

ISL::Map Var_t::createMap(Poly_scope_t& scope_object, ISL::Set func_set){
	this->scope(scope_object);
	ISL::Map map = isl_map_universe(scope_object.map_space(isl_set_dim(func_set, isl_dim_set), this->dim_size()));
	map = isl_map_intersect_domain(map, ISL::copy(func_set));
	return this->createMap(map);
}

ISL::Map Var_t::createMap(ISL::Map map){
	this->check_inner("create a map");
	fill_constraint(map);
	map = inner->createMap(map);

	return project_out_var(map);
}

size_t Var_t::dim_size(){
	this->check_inner("gives poly sizes");
	return inner->dim_size();
}

void Var_t::scope(Poly_scope_t& scope_object){
	this->check_inner("scope the poly");
	scope_object.add_var(name);
	value->scope(scope_object);
	inner->scope(scope_object);
}

Var_t* Var_t::clone() const{
	if(inner == nullptr)
		return new Var_t(name, value->clone(), nullptr);
	else
		return new Var_t(name, value->clone(), inner->clone());
}

void Var_t::check_inner(const std::string& msg){
	if(inner == nullptr){
		std::cout << "***Error: Cannot " << msg << ", var \"" << name << "\" has a empty poly !\n";
		exit(0);
	}
}

void Var_t::print() const{
	std::cout << name << "=";
	value->print(); 
	if(inner != nullptr)
		inner->print();
}