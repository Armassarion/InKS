#include <Declarations/Buffer_class_decl_t.hpp>
#include <Codes/Function_call_t.hpp>
#include <Codes/Buffer_class_t.hpp>
#include <Scope_exception_t.hpp>
#include <List_named_poly_t.hpp>
#include <isl_processing.hpp>
#include <const_globals.hpp>
#include <Poly_scope_t.hpp>
#include <Set_Poly_t.hpp>
#include <DD_Copy_t.hpp>
#include <isl_utils.hpp>
#include <DD_Gen_t.hpp>
#include <Implem_t.hpp>
#include <Point_t.hpp>
#include <Array.hpp>

DD_Copy_t::DD_Copy_t(int line, List_named_poly_t* _set) : 
	DD_Line_t(line),
	set(_set)
{}

uint DD_Copy_t::copy_id = 0;
	
void DD_Copy_t::generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
	ISL::UMap Wmap = events.map_from_time_to_write(time[Modification_t::Type::WritePhy]);
	ISL::Map wmap = isl_map_from_union_map(ISL::copy(Wmap));
	ISL::UMap rmap = events.map_from_time_to_read(time[Modification_t::Type::WritePhy]);
	std::string lid(isl_map_get_tuple_name(wmap, isl_dim_in));
		
	//Because params could appear from others loops, have to investigate why exactly but here's a quick fix
	{
		int nb_param = isl_set_dim(context, isl_dim_param);
		int wmap_param = isl_map_dim(wmap, isl_dim_param);
		wmap = isl_map_project_out(wmap, isl_dim_param, nb_param, wmap_param-nb_param);
	}
	
	std::string Wlid(lid + "_out_" + std::to_string(++copy_id));
	implem.push_decl(new Buffer_class_decl_t(Wlid, my_scope.array(lid).type, my_scope.get_const(), Wmap));
	implem.push_code(new Buffer_class_t(Wlid, Wmap, my_scope.get_const()));
	
	std::string Rlid(lid + "_in_" + std::to_string(copy_id));
	implem.push_decl(new Buffer_class_decl_t(Rlid, my_scope.array(lid).type, my_scope.get_const(), rmap));
	implem.push_code(new Buffer_class_t(Rlid, rmap, my_scope.get_const()));
	
	std::string copy_fct = "copy_" + pid + "_" + std::to_string(copy_id);
		
	wmap = isl_map_reverse(wmap);
	wmap = isl_map_set_tuple_name(wmap, isl_dim_in, copy_fct.c_str());
	generate_copy_fct(wmap, implem, my_scope.get_const(), context);
	
	implem.push_code(new Function_call_t(copy_fct, {Wlid, Rlid, my_scope.get_const().get_params()}));
}

void DD_Copy_t::check(const Time_Event_t& events){
}

void DD_Copy_t::scope(DD_Scope_t& scope_object){
	my_scope = scope_object;
	{
		area = set->createSet(scope_object.get_const(), scope_object.get_list_vars());
		pid = std::string(isl_set_get_tuple_name(isl_set_from_union_set(ISL::copy(area))));
	}
	
	try{
		scope_object.modifications.push_writephy(this, area);
	}catch(Scope_exception_t& se){
		throw Scope_exception_t(lineno, se.what());
	}
}
