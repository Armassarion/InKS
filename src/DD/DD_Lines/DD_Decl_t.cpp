#include <iostream>
#include <sstream>

#include <Scope_exception_t.hpp>
#include <Codes/Variable_t.hpp>
#include <DD_Scope_t.hpp>
#include <isl_utils.hpp>
#include <DD_Decl_t.hpp>
#include <Implem_t.hpp>
#include <DD_Gen_t.hpp>
#include <utils.hpp>

DD_Decl_t::DD_Decl_t(const DD_Decl_t& src) : 
	DD_Line_t(src.lineno),
	type(src.type),
	id(src.id),
	dim(src.dim)
{}

void DD_Decl_t::generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
	implem.push_code(new Variable_t(id, type, dim));
}

void DD_Decl_t::check(const Time_Event_t& events){
}

void DD_Decl_t::scope(DD_Scope_t& scope_object){
	if(scope_object.decl_contains(id))
		throw Scope_exception_t(id + " is already declared\n");
	
	scope_object.add_decl(*this);
	my_scope = scope_object;
	
	scope_object.print("Var", lineno);
}

bool DD_Decl_t::match(const std::string& src_type, int src_dim) const{
	return type == src_type && dim == src_dim;
}

int DD_Decl_t::get_dim() const{
	return dim;
}

std::string DD_Decl_t::get_physical_id() const{
	return id;
}