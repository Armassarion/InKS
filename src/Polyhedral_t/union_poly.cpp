#include <Poly_scope_t.hpp>
#include <Union_Poly_t.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>

Union_Poly_t::Union_Poly_t()
{}

Union_Poly_t::Union_Poly_t(Set_Poly_t* p){
	polys.push_back(p);
}

Union_Poly_t* Union_Poly_t::push(Set_Poly_t* p){
	polys.push_back(p);
	return this;
}

size_t Union_Poly_t::dim_size(){
	return polys.empty() ? 0 : polys[0]->dim_size();
}

ISL::Set Union_Poly_t::createSet(const V_Input_t& Vin){
	Poly_scope_t polyscope(Vin);
	return this->createSet(polyscope);
}

ISL::Set Union_Poly_t::createSet(Poly_scope_t& scope_object){
	this->scope(scope_object);
	return this->createSet(isl_set_universe(scope_object.param_space()));
}

ISL::Set Union_Poly_t::createSet(ISL::Set set){
	ISL::Set rset = empty_isl_set(this->dim_size());
	for(Set_Poly_t* p : polys)
		rset = isl_set_union(rset, p->createSet(ISL::copy(set)));
		
	return isl_set_coalesce(rset);
}

ISL::Map Union_Poly_t::createMap(const V_Input_t& Vin, ISL::Set func_set){
	Poly_scope_t polyscope(Vin, func_set);
	return this->createMap(polyscope, func_set);
}

ISL::Map Union_Poly_t::createMap(Poly_scope_t& scope_object, ISL::Set func_set){
	this->scope(scope_object);
	return this->createMap(isl_map_universe(scope_object.map_space(isl_set_dim(func_set, isl_dim_set), this->dim_size())));
}

ISL::Map Union_Poly_t::createMap(ISL::Map map){
	ISL::Map rmap = isl_map_empty(isl_map_get_space(map));
	for(Set_Poly_t* p : polys)
		rmap = isl_map_union(rmap, p->createMap(ISL::copy(map)));

	return isl_map_coalesce(rmap);
}

void Union_Poly_t::scope(Poly_scope_t& scope_object){
	for(Set_Poly_t* p : polys)
		p->scope(scope_object);
}

Set_Poly_t* Union_Poly_t::clone() const{
	Union_Poly_t* r = new Union_Poly_t();
	for(const Set_Poly_t* s : polys)
		r->push(s->clone());
	return r;
}

void Union_Poly_t::print() const{
	std::cout << "{\n";
	for(const Set_Poly_t* s : polys){
		std::cout << "\t";
		s->print();
	}
	std::cout << "\n}\n";
}