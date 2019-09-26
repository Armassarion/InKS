#include <Poly_scope_t.hpp>
#include <DD_Unmap_t.hpp>
#include <Set_Poly_t.hpp>
#include <isl_utils.hpp>
#include <DD_Gen_t.hpp>


DD_Unmap_t::DD_Unmap_t(int line, const char* _pid, Set_Poly_t* _set) : 
	DD_Line_t(line),
	pid(_pid),
	set(_set)
{}

void DD_Unmap_t::check(const Time_Event_t& events){
	
}

void DD_Unmap_t::scope(DD_Scope_t& scope_object){
	ISL::Set area;

	if(set == nullptr){
		area = universe_isl_set(scope_object.decl_dim_size(pid));
	}else{
		Poly_scope_t poly_scope(scope_object.get_const());
		area = scope_object.get_poly(set)->createSet(poly_scope);
	}
	area = isl_set_set_tuple_name(area, pid.c_str());
	scope_object.modifications.push_unmap(this, area);
	my_scope = scope_object;
	
	scope_object.print("Unmap", lineno);
}

void DD_Unmap_t::generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
	//Nothing to do
}