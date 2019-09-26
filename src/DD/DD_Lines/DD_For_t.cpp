#include <functional>
#include <iostream>
#include <tuple>

#include <DDFor_utils/V_DDForNamedVar.hpp>
#include <Scope_exception_t.hpp>
#include <List_named_poly_t.hpp>
#include <isl_processing.hpp>
#include <const_globals.hpp>
#include <Poly_scope_t.hpp>
#include <Codes/For_t.hpp>
#include <DD_Code_t.hpp>
#include <Array_Map.hpp>
#include <isl_utils.hpp>
#include <DD_Gen_t.hpp>
#include <DD_For_t.hpp>
#include <List_t.hpp>
#include <utils.hpp>
	
DD_For_t::DD_For_t(int _lineno, List_named_poly_t* _data_out, V_DDForNamedVar* _order, DD_Code_t* _inner) : 
	DD_Line_t(_lineno),
	data_out(_data_out),
	order(_order),
	inner(_inner)
{}

void DD_For_t::generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
	
	ISL::Ast_build ab = isl_ast_build_set_iterators(isl_ast_build_from_context(ISL::copy(context)), order->to_id_list());
	ISL::Ast_node ast = isl_ast_build_node_from_schedule_map(ab, isl_union_map_from_map(ISL::copy(order_map)));
	Implem_t inner_implem;
	
	//ISL::print(context, "Before Context");
	context = isl_set_intersect_params(context, this->get_iteration_context());
	//ISL::print(context, "Context");
	
	inner->generate(inner_implem, events, ISL::copy(context));
	
	List_t* indexes = order->to_list();
	For_t* previous = nullptr;
	For_t* first_loop;
	for(int i=0; i<order->size(); i++){
		std::string indice = indexes->get(i);
		ISL::Ast_expr min = isl_ast_node_for_get_init(ast);
		ISL::Ast_expr cond = isl_ast_node_for_get_cond(ast);
		ISL::Ast_expr inc = isl_ast_node_for_get_inc(ast);
		ast = isl_ast_node_for_get_body(ast);
		
		For_t* ith_loop = new For_t(indice, min, cond, inc);
		
		ith_loop->set_before_outside(new Str_code_t(order->add_str_before(indice)));
		//ith_loop->set_after_inside(new Str_code_t(order->add_str_before(indice)));
		
		if(previous != nullptr)
			previous->set_inner(ith_loop);
		else
			first_loop = ith_loop;
			
		previous = ith_loop;
	}
	previous->set_inner(new Str_code_t(inner_implem.get_code_cpp_string()));

	implem.push_decl(inner_implem.declarations);
	implem.push_code(first_loop);
}

void DD_For_t::schedule(DD_Scope_t& scope_object){

}

void DD_For_t::scope(DD_Scope_t& scope_object){
	
	DD_Scope_t inner_scope(scope_object);
	my_scope = scope_object;

	inner_scope.add_const(order->to_list());
	
	order_map_param = order->get_order_map_param(my_scope);
	order_map = order->get_order_map(my_scope);
	
	ISL::Set forvars = isl_map_domain(order->get_order_map_param(my_scope));
	forvars = isl_set_set_tuple_name(forvars, "");
	
	auto& original_time = scope_object.modifications.push_addfor(this, ISL::copy(forvars), inner->nb_action());
	inner->scope(inner_scope);
	scope_object.modifications += inner_scope.modifications;
	scope_object.modifications.push_delfor(this, ISL::copy(forvars), original_time);
}

void DD_For_t::check(const Time_Event_t& events){	

}

ISL::UMap DD_For_t::getRel(ISL::USet data) const{
	V_Input_t v(my_scope.get_const());
	v.merge(order->to_list());
	
	ISL::Set for_set = isl_map_domain(ISL::copy(order_map_param));
	ISL::UMap r = empty_isl_union_map();
	
	ISL::L_Set lset = isl_union_set_get_set_list(data);
	int nb = isl_set_list_size(lset);
	for(int i=0; i<nb; ++i){
		ISL::Map m = isl_map_from_domain_and_range(ISL::copy(for_set), isl_set_list_get_set(lset, i));
		m = isl_map_project_out(m, isl_dim_param, my_scope.const_size(), order->size());
		r = isl_union_map_union(r, isl_union_map_from_map(m));
	}
	
	return isl_union_map_coalesce(r);
}

ISL::Set DD_For_t::get_iteration_context(){
	return isl_set_coalesce(isl_map_params(order->get_order_map_param(my_scope)));
}

void DD_For_t::resolve(std::vector<DD_Line_t*>& new_lines){
	DD_Line_t::resolve(new_lines);
	inner->resolve();
}