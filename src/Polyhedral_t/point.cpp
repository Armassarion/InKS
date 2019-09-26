#include <iostream>
#include <string.h>
#include <vector>

#include <Constraint_t.hpp>
#include <Poly_scope_t.hpp>
#include <Reference_t.hpp>
#include <Equality_t.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>
#include <Point_t.hpp>
#include <List_t.hpp>

using std::vector;
using std::cout;
using std::cerr;
using std::endl;

Point_t::Point_t()
{}

Point_t::Point_t(Constraint_t* c){
	pt.push_back(c);
}

Point_t::Point_t(List_t* l){
	for(auto& param : *l){
		Reference_t* ref = new Reference_t(strdup(param.c_str()));
		Equality_t* eq = new Equality_t(ref);
		pt.push_back(eq);
	}
}

Point_t* Point_t::push(Constraint_t* e){
	pt.push_back(e);
	return this;
}

Point_t* Point_t::push(const char* rname){
	pt.push_back(new Equality_t(new Reference_t(rname)));
	return this;
}

size_t Point_t::dim_size(){
	return pt.size();
}

ISL::Set Point_t::createSet(const V_Input_t& Vin){
	Poly_scope_t polyscope(Vin);
	return this->createSet(polyscope);
}

ISL::Set Point_t::createSet(Poly_scope_t& scope_object){
	this->scope(scope_object);
	return this->createSet(isl_set_universe(scope_object.param_space()));
}


ISL::Set Point_t::createSet(ISL::Set set){
	vector<ISL::Constraint> cs;
	set = isl_set_add_dims(set, isl_dim_set, this->dim_size());
	
	for(size_t dim = 0; dim<pt.size(); dim++){
		cs.clear();
		
		if(pt[dim]->is_modulo()) set = isl_set_add_dims(set, isl_dim_set, 1);
		pt[dim]->init_lspace(lspace_from_set(set));
		pt[dim]->create_constraint(cs, dim, Constraint_t::TYPE_ISL::Set);
		for(ISL::Constraint c : cs)
			set = isl_set_add_constraint(set, c);
		if(pt[dim]->is_modulo()) set = isl_set_project_out(set, isl_dim_set, isl_set_dim(set, isl_dim_set)-1, 1);
	}
	return set;
}


ISL::Map Point_t::createMap(const V_Input_t& Vin, ISL::Set func_set){
	Poly_scope_t polyscope(Vin, func_set);
	return this->createMap(polyscope, ISL::copy(func_set));
}

ISL::Map Point_t::createMap(Poly_scope_t& scope_object, ISL::Set func_set){
	this->scope(scope_object);
	ISL::Map map = isl_map_universe(scope_object.map_space(isl_set_dim(func_set, isl_dim_set), this->dim_size()));
	map = isl_map_intersect_domain(map, ISL::copy(func_set));
	return this->createMap(map);
}
	
ISL::Map Point_t::createMap(ISL::Map map){
	vector<ISL::Constraint> cs;
	for(size_t dim = 0; dim<pt.size(); dim++){
		cs.clear();
		
		if(pt[dim]->is_modulo()) map = isl_map_add_dims(map, isl_dim_out, 1);
		pt[dim]->init_lspace(lspace_from_map(map));
		pt[dim]->create_constraint(cs, dim, Constraint_t::TYPE_ISL::Out);
		for(ISL::Constraint c : cs){
			map = isl_map_add_constraint(map, c);
		}
		if(pt[dim]->is_modulo()) map = isl_map_project_out(map, isl_dim_out, isl_map_dim(map, isl_dim_out)-1, 1);
	}
	return map;
}

void Point_t::scope(Poly_scope_t& scope_object){
	for(Constraint_t* c : pt)
		c->scope(scope_object);
}

Set_Poly_t* Point_t::clone() const{
	Point_t* r = new Point_t();
	for(auto& c : pt)
		r->push(c->clone());
	return r;
}

void Point_t::print() const{
	std::cout << "(";
	for(size_t i=0; i<pt.size(); ++i){
		pt[i]->print();
		if(i + 1 < pt.size()) std::cout << ", ";
	}
	std::cout << ")\n";
}