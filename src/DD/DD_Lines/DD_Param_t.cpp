#include <iostream>
#include <sstream>

#include <Codes/Mult_array_t.hpp>
#include <Scope_exception_t.hpp>
#include <DD_Param_t.hpp>
#include <Set_Poly_t.hpp>
#include <DD_Scope_t.hpp>
#include <isl_utils.hpp>
#include <DD_Decl_t.hpp>
#include <DD_Gen_t.hpp>
#include <Implem_t.hpp>
#include <DD_Gen_t.hpp>
#include <DD_Map_t.hpp>
#include <Point_t.hpp>
#include <utils.hpp>

IOMap_t::IOMap_t(DD_Map_t* _in, DD_Map_t* _out) :
	in(_in),
	out(_out)
{}

DD_Param_t::DD_Param_t(int _lineno, OWN _prop, const char * _type, const char* _pid, Set_Poly_t* _sizes, IOMap_t* _io) : 
	DD_Mem_t(_lineno, _type, _pid, _sizes, _prop),
	io_maps(_io)
{
	if(is_give() && io_maps->in != nullptr){
		std::cerr << "***Error: cannot have a \"give\" param with an input map." << std::endl;
		exit(EXIT_FAILURE);
	}
}

DD_Param_t::DD_Param_t(int _lineno, OWN _prop, const char * _type, const char* _pid, IOMap_t* _io) : 
	DD_Param_t(_lineno, _prop, _type, _pid, new Point_t(), _io)
{}

void DD_Param_t::generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
	implem.push_io(IO_t(physical_id, type, get_dim_size(), this->is_keep()));
	
	if(this->is_keep() || this->is_take())
		implem.push_code(new Mult_array_t(physical_id, sizes_isl));
	if(io_maps->in != nullptr)
		io_maps->in->generate(implem, events, context);
}

void DD_Param_t::check(const Time_Event_t& events){
	
}

void DD_Param_t::scope(DD_Scope_t& scope_object){
	DD_Decl_t decl(get_lineno(), get_type(), get_physical_id(), get_dim_size());
	decl.scope(scope_object);
	DD_Mem_t::scope(scope_object);
	
	if(io_maps->in != nullptr){
		io_maps->in->scope(scope_object);
		scope_object.modifications.push_write(this, isl_union_set_from_set(isl_map_domain(io_maps->in->get_map())));
	}
		
	scope_object.print("Param", lineno);
}
