#include <iostream>
#include <sstream>

#include <Codes/Mult_array_t.hpp>
#include <Scope_exception_t.hpp>
#include <Codes/Buffer_t.hpp>
#include <const_globals.hpp>
#include <Poly_scope_t.hpp>
#include <DD_Scope_t.hpp>
#include <Set_Poly_t.hpp>
#include <isl_utils.hpp>
#include <DD_Mem_t.hpp>
#include <Implem_t.hpp>
#include <DD_Gen_t.hpp>
#include <Point_t.hpp>
#include <utils.hpp>

DD_Mem_t::DD_Mem_t(const DD_Mem_t& src){
	if(src.sizes_isl == nullptr){
		std::cerr << "Line " << lineno << " : sizes isl shouldnt be null to be copied" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	lineno = src.lineno;
	own_prop = src.own_prop;
	type = src.type;
	physical_id = src.physical_id;
	sizes = nullptr;
	sizes_isl = ISL::copy(src.sizes_isl);
}

void DD_Mem_t::generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
	const uint nb_dim = isl_set_dim(sizes_isl, isl_dim_set);
	if(nb_dim > 0){
		implem.push_code(new Buffer_t(physical_id, type, sizes_isl, nb_dim));
		implem.push_code(new Mult_array_t(physical_id, sizes_isl));
	}
}

void DD_Mem_t::check(const Time_Event_t& events){
}

void DD_Mem_t::scope(DD_Scope_t& scope_object){
	if(!is_give()){
		if(!scope_object.decl_contains(physical_id, type, sizes->dim_size()))
			throw Scope_exception_t(lineno, physical_id + " variable isn't declared or type/dimension number mismatch\n");
		if(scope_object.alloc_contains(physical_id))
			throw Scope_exception_t(lineno, physical_id + " buffer already exists in the scope\n");
		
		Poly_scope_t poly_scope(scope_object.get_const());
		sizes_isl = scope_object.get_poly(sizes)->createSet(poly_scope);
		
		if(plus_one){
			ISL::Set r = isl_set_universe(isl_set_get_space(sizes_isl));
			ISL::L_BSet bl = isl_set_get_basic_set_list(sizes_isl);
			for(int i=0; i<isl_basic_set_list_size(bl); i++){
				ISL::BSet bs = isl_basic_set_list_get_at(bl, i);
				ISL::L_Constraint cl = isl_basic_set_get_constraint_list(bs);
				for(int j=0; j<isl_constraint_list_size(cl); j++){
					ISL::Constraint c = isl_constraint_list_get_at(cl, j);
					c = isl_constraint_set_constant_val(c, isl_val_add(isl_constraint_get_constant_val(c), isl_val_one(ctx)));
					r = isl_set_add_constraint(r, c);
				}
			}
			sizes_isl = r;
		}
		
		if(is_keep())
			scope_object.add_mem_keep(*this);
		else
			scope_object.add_mem(*this);
		my_scope = scope_object;
	}
	
	scope_object.print("Alloc", lineno);
}

std::string DD_Mem_t::get_param(){
	if(sizes->dim_size() == 0)
		return type + " " + physical_id;
	else
		return type + "* " + physical_id;
}

ISL::PWAff DD_Mem_t::dim_size(uint i){
	if(sizes_isl != nullptr && i >= 0 && i < isl_set_dim(sizes_isl, isl_dim_set))
		return isl_set_dim_max(ISL::copy(sizes_isl), i);
	else
		throw Scope_exception_t(lineno, "*** ERROR: Buffer \"out of dim\" error\n");
}

DD_Mem_t& DD_Mem_t::operator=(DD_Mem_t& _m){
	if(_m.sizes != nullptr)
		this->sizes = _m.sizes;
	if(_m.sizes_isl != nullptr)
		this->sizes_isl = ISL::copy(_m.sizes_isl);
	
	this->physical_id = _m.physical_id;
	this->type = _m.type;
	
	return *this;
}

void DD_Mem_t::print() const{
	std::cout << "Alloc: " << type << " " << physical_id << " = " << isl_set_to_str(sizes_isl) << std::endl;
}

size_t DD_Mem_t::get_dim_size() const{
	return sizes->dim_size();
}

std::string DD_Mem_t::get_sizes() const{
	std::vector<std::vector<C_from_isl_t>> sizes_str(std::move(set_component_to_string(ISL::copy(sizes_isl))));
	uint nb = sizes_str.size();
	std::ostringstream r;
	
	r << "{{";
	for(uint i=0; i<nb; ++i)
		r << sizes_str[i][0].val << add_coma_if(i, nb);
	r << "}}";
	
	return r.str();
}

std::string DD_Mem_t::get_mult() const{
	const uint nb = isl_set_dim(sizes_isl, isl_dim_set);
	if(nb == 0) return "";
	if(nb == 1) return "{{1}}";
	
	std::vector<std::vector<C_from_isl_t>> sizes_str(std::move(set_component_to_string(ISL::copy(sizes_isl))));
	std::vector<std::string> vec_mult(nb-1, sizes_str[nb-1][0].val);
	std::ostringstream r;
	
	for(uint i=1; i<nb-1; i++)
		vec_mult[i] = vec_mult[i-1] + " * " + sizes_str[nb-i-1][0].val;
	
	r << "{{";
	for(uint i=0; i<nb-1; ++i)
		r << vec_mult[nb-i-2] << ", ";
	r << "1}}";
	
	return r.str();
}

std::string DD_Mem_t::get_physical_id() const{
	return physical_id;
}

std::string DD_Mem_t::get_type() const{
	return type;
}

bool DD_Mem_t::is_give() const{
	return own_prop == OWN::Give;
}

bool DD_Mem_t::is_keep() const{
	return own_prop == OWN::Keep;
}

bool DD_Mem_t::is_take() const{
	return own_prop == OWN::Take;
}

