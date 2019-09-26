#include <algorithm>
#include <iostream>

#include <V_Function_application_t.hpp>
#include <Function_t.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>

using std::cerr;
using std::cout;
using std::endl;

V_Function_application_t::V_Function_application_t()
{}

V_Function_application_t::V_Function_application_t(Function_application_t* _fa){
	vfa.push_back(_fa);
}

void V_Function_application_t::scope(Function_Map& Functions, Array_Map& Arrays){
	for(Function_application_t* fa : vfa)
		fa->scope(Functions, Arrays);
}

V_Function_application_t* V_Function_application_t::push(Function_application_t* _fa){
	vfa.push_back(_fa);
	return this;
}

Function_application_t* V_Function_application_t::get(size_t i){
	return vfa[i];
}

ISL::USet V_Function_application_t::createApplySet(const V_Input_t& Vin, List_var_t& vars){
	ISL::USet ApplySet = empty_isl_union_set();
	for(auto& d : vfa)
		ApplySet = isl_union_set_union(ApplySet, isl_union_set_from_set(d->createIslSet(Vin, vars)));
	return isl_union_set_coalesce(ApplySet);
}

ISL::UMap V_Function_application_t::get_InRelation(Function_application_t::Instance_name_mode name_mode){
	ISL::UMap r = empty_isl_union_map();
	for(Function_application_t* fa : vfa)
		r = isl_union_map_union(r, fa->createInRelation(name_mode));
	return isl_union_map_coalesce(r);
}

ISL::UMap V_Function_application_t::get_OutRelation(Function_application_t::Instance_name_mode name_mode){
	ISL::UMap r = empty_isl_union_map();
	for(Function_application_t* fa : vfa)
		r = isl_union_map_union(r, fa->createOutRelation(name_mode));
	return isl_union_map_coalesce(r);
}

size_t V_Function_application_t::size() const{
	return vfa.size();
}

Function_application_t* V_Function_application_t::find_from_func_name(std::string _fname){
	Fa_iterator it = std::find_if(vfa.begin(), vfa.end(), [&](Function_application_t* fa){
		return fa->refer_to(_fname);
	});
	if(it == vfa.end()) return nullptr;
	return *it;
}

Function_application_t* V_Function_application_t::find_from_instance_name(std::string _fname){
	Fa_iterator it = std::find_if(vfa.begin(), vfa.end(), [&](Function_application_t* fa){
		return fa->instance() == _fname;
	});
	if(it == vfa.end()) return nullptr;
	return *it;
}

Fill_argument_res V_Function_application_t::fill_argument(std::string key){
	for(Function_application_t* fa : vfa)
		if(fa->refer_to(key))
			return fa->fill_argument();
	std::cerr << "***Error (filling arguments): No kernel named " << key << std::endl;
	exit(EXIT_FAILURE);
}

size_t V_Function_application_t::get_array_index(std::string array_name){
	for(Function_application_t* fa : vfa){
		int r = fa->get_array_index(array_name);
		if(r != -1) return r;
	}
	throw std::out_of_range("V_Function_application_t::get_array_index");
}

Function_application_t* V_Function_application_t::func_from_instance(std::string instance_name){
	for(Function_application_t*& fa : vfa)
		if(fa->instance_name == instance_name)
			return fa;
	return nullptr;
}
