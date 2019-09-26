#include <iostream>
#include <sstream>

#include <Codes/Exit_t.hpp>
#include <Codes/If_t.hpp>

#include <isl_processing.hpp>
#include <DD_Block_t.hpp>
#include <DD_Param_t.hpp>
#include <DD_Code_t.hpp>
#include <DD_Decl_t.hpp>
#include <isl_utils.hpp>
#include <Array_Map.hpp>
#include <DD_Gen_t.hpp>
#include <Implem_t.hpp>
#include <List_t.hpp>
#include <utils.hpp>

DD_Alloc_param_t::DD_Alloc_param_t(DD_Param_t* _d){
	alloc.push_back(_d);
}

DD_Alloc_param_t* DD_Alloc_param_t::push(DD_Param_t* _d){
	alloc.push_back(_d);
	return this;
}

void DD_Alloc_param_t::add_to_scope(DD_Scope_t& scope_object){
	for(DD_Param_t* param : alloc)
		param->scope(scope_object);
}

void DD_Alloc_param_t::generate_io(Implem_t& implem, const Time_Event_t& events){
	for(DD_Param_t* param : alloc)
		param->generate(implem, events, ISL::copy(valid_parameters));
}
		
std::string DD_Alloc_param_t::get_alloc_params() const{
	std::ostringstream r;
	for(uint i=0; i<alloc.size(); ++i)
		r << alloc[i]->get_param() << add_coma_if(i, alloc.size());
	return r.str();
}

DD_Block_t::DD_Block_t(const char* _id, DD_Alloc_param_t* _alloc, List_t* _const, DD_Code_t* _inner) :
	id(_id),
	alloc_params(_alloc),
	const_params(_const),
	inner(_inner)
{}

DD_Block_t::DD_Block_t(const char* _id, DD_Alloc_param_t* _allocs, DD_Code_t* _inner) :
	DD_Block_t(_id, _allocs, new List_t(), _inner)
{}

DD_Block_t::DD_Block_t(const char* _id, List_t* _const, DD_Code_t* _inner) :
	DD_Block_t(_id, new DD_Alloc_param_t(), _const, _inner)
{}

void DD_Block_t::resolve(){
	inner->resolve();
}
	
void DD_Block_t::scope(ISL::Set valid_parameters, Array_Map* Arrays, ISL::USet data_in){
	my_scope.init(valid_parameters, Arrays, const_params, data_in);
	my_scope.print("Block before ");

	alloc_params->add_to_scope(my_scope);
	
	my_scope.print("Block alloc ");
	inner->scope(my_scope);
	my_scope.print("Block after ");
	
	events = my_scope.modifications.get_SchEvent(ISL::copy(valid_parameters), Arrays->max_logical_sets());
}

void DD_Block_t::scope(ISL::Set valid_parameters, Array_Map* Arrays){
	this->scope(valid_parameters, Arrays, empty_isl_union_set());
}

Implem_t DD_Block_t::generate(){
	std::ostringstream r;
	Implem_t implem(id, const_params);
	
	alloc_params->generate_io(implem, events);
	inner->generate(implem, events);
	
	return implem;
}

bool DD_Block_t::is_the_chosen_one(const std::string& public_id, const V_Input_t& public_param) const {
	return (id == public_id && const_params == public_param);
}
