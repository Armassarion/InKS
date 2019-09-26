#include <functional>
#include <sstream>

#include <Declarations/Buffer_class_decl_t.hpp>
#include <Codes/Buffer_class_t.hpp>
#include <Scope_exception_t.hpp>
#include <isl_processing.hpp>
#include <const_globals.hpp>
#include <Poly_scope_t.hpp>
#include <Set_Poly_t.hpp>
#include <isl_utils.hpp>
#include <Array_Map.hpp>
#include <DD_Map_t.hpp>
#include <DD_Gen_t.hpp>
#include <Implem_t.hpp>
#include <DD_Mem_t.hpp>
#include <Point_t.hpp>
#include <List_t.hpp>
#include <Var_t.hpp>

DD_Map_t::DD_Map_t(int _lineno, List_var_t* _rl, const char* _pid, Set_Poly_t* _ps, const char* _lid, Set_Poly_t* _ls) :
	DD_Line_t(_lineno),
	vars(_rl),
	physical_id(_pid),
	physical_set(_ps),
	logical_id(_lid),
	logical_set(_ls),
	map(nullptr)
{}

DD_Map_t::DD_Map_t(int _lineno, List_var_t* _rl, const char* _pid, const char* _lid, Set_Poly_t* _ls) :
	DD_Line_t(_lineno),
	vars(_rl),
	physical_id(_pid),
	physical_set(new Point_t()),
	logical_id(_lid),
	logical_set(_ls),
	map(nullptr)
{}

DD_Map_t::DD_Map_t(int _lineno, const char* _pid, Set_Poly_t* _ps, const char* _lid, Set_Poly_t* _ls) :
	DD_Line_t(_lineno),
	vars(new List_var_t()),
	physical_id(_pid),
	physical_set(_ps),
	logical_id(_lid),
	logical_set(_ls),
	map(nullptr)
{}

DD_Map_t::DD_Map_t(int _lineno, const char* _pid, const char* _lid, Set_Poly_t* _ls) :
	DD_Line_t(_lineno),
	vars(new List_var_t()),
	physical_id(_pid),
	physical_set(new Point_t()),
	logical_id(_lid),
	logical_set(_ls),
	map(nullptr)
{}

DD_Map_t::DD_Map_t(int _lineno, const char* _pid, const char* _lid) :
	DD_Line_t(_lineno),
	vars(new List_var_t()),
	physical_id(_pid),
	physical_set(new Point_t()),
	logical_id(_lid),
	logical_set(new Point_t()),
	map(nullptr)
{}

void DD_Map_t::generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
	
}

void DD_Map_t::check(const Time_Event_t& events) {

}
	

void DD_Map_t::scope(DD_Scope_t& scope_object){
	if(!scope_object.logical_contains(logical_id))
		throw Scope_exception_t(lineno, std::string("Logical array \"") + logical_id + std::string("\" does not exist.\n"));
		
	if(!scope_object.alloc_contains(physical_id))
		throw Scope_exception_t(lineno, std::string("Logical array \"") + logical_id + std::string("\" can't be mapped to unknown buffer \"") + physical_id + std::string("\".\n"));
	
	this->compute_map(scope_object);

	//if(!debug_mode)
	//	scope_object.gist_maps(map);

	scope_object.modifications.push_unmap(this, isl_map_range(ISL::copy(map)));
	scope_object.modifications.push_map(this, ISL::copy(map));
	my_scope = scope_object;
	
	scope_object.print("Map", lineno);
}

ISL::Map DD_Map_t::compute_map(DD_Scope_t& scope_object){
	//Map log->phy
	{
		List_t range_list = vars->get_var_names().merge(scope_object.get_var_names());
		ISL::Map range_to_log;
		ISL::Map range_to_phy;
		ISL::Set range_set;
		{
			Point_t p(&range_list);
			Set_Poly_t* for_poly = scope_object.get_poly(vars->get_poly(&p));
			Poly_scope_t poly_scope(scope_object.get_const());
			range_set = for_poly->createSet(poly_scope);
		}
		{
			Poly_scope_t poly_scope(scope_object.get_const(), &range_list);
			range_to_log = scope_object.get_poly(logical_set)->createMap(poly_scope, ISL::copy(range_set));
		}
		{
			Poly_scope_t poly_scope(scope_object.get_const(), &range_list);
			range_to_phy = scope_object.get_poly(physical_set)->createMap(poly_scope, ISL::copy(range_set));
		}
		map = isl_map_apply_range(isl_map_reverse(range_to_log), range_to_phy);
	}
	map = isl_map_set_tuple_name(map, isl_dim_out, physical_id.c_str());
	map = isl_map_set_tuple_name(map, isl_dim_in, logical_id.c_str());
	map = isl_map_intersect_params(map, ISL::copy(valid_parameters));
	return ISL::copy(map);
}

ISL::Map DD_Map_t::get_map() const{
	if(map == nullptr)
		throw Scope_exception_t(lineno, "Cannot return a nullptr map !");
	return ISL::copy(map);
}

std::string DD_Map_t::get_physical_id() const{
	return physical_id;
}
	
std::string DD_Map_t::get_logical_id() const{
	return logical_id;
}

size_t DD_Map_t::get_physical_dim_size() const{
	return physical_set->dim_size();
}

Set_Poly_t* DD_Map_t::get_physical_set() const{
	if(vars->size() == 0) return physical_set->clone();
	
	Var_t* r = vars->get(0);
	Var_t* current = r;
	for(size_t i=1; i<vars->size(); i++){
		Var_t* next = vars->get(i);
		current->set_inner(next);
		current = next;
	}
	
	current->set_inner(physical_set->clone());
	return r;
}