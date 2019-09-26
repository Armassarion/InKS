#include <Named_Poly_t.hpp>
#include <Poly_scope_t.hpp>
#include <isl_utils.hpp>
#include <Point_t.hpp>
#include <Input_t.hpp>

Named_Poly_t::Named_Poly_t()
{}

Named_Poly_t::Named_Poly_t(const char* _n, Set_Poly_t* _p) : 
	name(_n),
	poly(_p)
{}

Named_Poly_t::Named_Poly_t(const char* _n) : 
	name(_n),
	poly(new Point_t())
{}

size_t Named_Poly_t::dim_size(){
	return poly->dim_size();
}

ISL::Set Named_Poly_t::createSet(const V_Input_t& Vin){
	Poly_scope_t polyscope(Vin);
	return this->createSet(polyscope);
}

ISL::Set Named_Poly_t::createSet(Poly_scope_t& scope_object){
	this->scope(scope_object);
	return this->createSet(isl_set_universe(scope_object.param_space()));
}

ISL::Set Named_Poly_t::createSet(ISL::Set set){
	return isl_set_set_tuple_name(poly->createSet(set), name.c_str());
}

ISL::Map Named_Poly_t::createMap(const V_Input_t& Vin, ISL::Set func_set){
	Poly_scope_t polyscope(Vin, func_set);
	return this->createMap(polyscope, func_set);
}

ISL::Map Named_Poly_t::createMap(Poly_scope_t& scope_object, ISL::Set func_set){
	this->scope(scope_object);
	return this->createMap(isl_map_universe(scope_object.map_space(isl_set_dim(func_set, isl_dim_set), this->dim_size())));
}

ISL::Map Named_Poly_t::createMap(ISL::Map map){
	return isl_map_set_tuple_name(isl_map_coalesce(poly->createMap(map)), isl_dim_out, name.c_str());
}

void Named_Poly_t::scope(Poly_scope_t& scope_object){
	poly->scope(scope_object);
}

Set_Poly_t* Named_Poly_t::clone() const{
	return new Named_Poly_t(name.c_str(), poly->clone());
}

void Named_Poly_t::print() const{
	std::cout << name;
	poly->print();
}