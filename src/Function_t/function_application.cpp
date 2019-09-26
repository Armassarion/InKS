#include <iostream>
#include <sstream>
#include <string>

#include <Function_application_t.hpp>
#include <Poly_Exception_t.hpp>
#include <V_Dependence_t.hpp>
#include <const_globals.hpp>
#include <Function_Map.hpp>
#include <Poly_scope_t.hpp>
#include <Function_t.hpp>
#include <Set_Poly_t.hpp>
#include <List_var_t.hpp>
#include <Array_Map.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>
#include <List_t.hpp>
#include <Array.hpp>

using oss = std::ostringstream;
using std::string;
using std::vector;
using std::cerr;
using std::endl;

std::map<std::string, int> Function_application_t::appl_map_id;

void Function_application_t::check_constructor(){
	oss r, base_error;
	base_error << "*** Error in InKS kernel application \"" << Func_ref->name << "\": ";
	
	if(Func_ref->dim_size() != set_dom->dim_size())
		r << "gets " << set_dom->dim_size() << " structuring value, expecting " << Func_ref->dim_size() << "." << endl;
	
	if(Func_ref->vio->size() != Param_arrays.size()){
		r << "gets " << Param_arrays.size() << " arrays, expecting " << Func_ref->vio->size() << "." << endl;
	}else{
		try{
			Func_ref->vio->check_arrays_dimsize(Param_arrays);
		}catch(V_Dependence_exception_t& vde){
			r << "array \"" << vde.name << "\" " <<  vde.dependence_type << " dimension size are not matching with the kernel application." << endl;
		}
	}
		
	if(!r.str().empty()){
		cerr << base_error.str() << r.str();
		exit(EXIT_FAILURE);
	}
}

Function_application_t::Function_application_t(std::string _ref, Set_Poly_t* _set, List_t* _al) :
	ref_name(_ref), 
	set_dom(_set), 
	array_list(_al), 
	set(nullptr)
{}

void Function_application_t::scope(Function_Map& Functions, Array_Map& Arrays){	
	Function_t& Func = Functions.at(string(ref_name));
	oss i_n;
	
	for(size_t i = 0; i<array_list->size(); ++i)
		Param_arrays.push_back(Arrays.get_ptr(string(array_list->get(i))));
		
	Func_ref = &Func;
	Func.add_application(this);
	
	if(!appl_map_id.count(Func_ref->name)) 
		appl_map_id[Func_ref->name] = 0;
	my_id = appl_map_id[Func_ref->name]++;
	i_n << Func_ref->name << "_" << my_id;

	instance_name = i_n.str();
	check_constructor();
}

ISL::Set Function_application_t::createIslSet(const V_Input_t& Vin, List_var_t& vars){
	try{
		set = vars.get_poly(set_dom)->createSet(Vin);
	}catch(Ref_Exception_t& pe){
		cerr << "***Error: unknown variable \"" + pe.unknown_variable + "\" in kernel application \"" +  Func_ref->name + "\" #" << my_id << "." << endl;
		exit(EXIT_FAILURE);
	}
	set = isl_set_set_tuple_name(set, instance_name.c_str());
	
	if(verbosity) cerr << Func_ref->name  << " set: " << ISL::str(set) << endl << endl;
	
	return ISL::copy(set);
}

ISL::UMap Function_application_t::createInRelation(Instance_name_mode name_mode){
	ISL::UMap r = empty_isl_union_map();
	for(size_t i=0; i<Param_arrays.size(); ++i)
		r = isl_union_map_union(r, set_relation_name(Func_ref->getIn(i), Param_arrays[i]->c_str(), name_mode));
	
	return r;
}

ISL::UMap Function_application_t::createOutRelation(Instance_name_mode name_mode){
	ISL::UMap r = empty_isl_union_map();
	for(size_t i=0; i<Param_arrays.size(); ++i)
		r = isl_union_map_union(r, set_relation_name(Func_ref->getOut(i), Param_arrays[i]->c_str(), name_mode));
	
	return r;
}

ISL::UMap Function_application_t::set_relation_name(ISL::Map R, const char* array_name, Instance_name_mode name_mode){
	ISL::Set domain = ISL::copy(set);
	if(name_mode == INSTANCE){
		R = isl_map_set_tuple_name(R, isl_dim_in, instance_name.c_str());
	}else{
		R = isl_map_set_tuple_name(R, isl_dim_in, ref_name.c_str());	
		domain = isl_set_set_tuple_name(domain, ref_name.c_str());
	}
	R = isl_map_set_tuple_name(R, isl_dim_out, array_name);
	R = isl_map_intersect_domain(R, domain);
	return isl_union_map_from_map(R);
}

string Function_application_t::instance() const {
	return instance_name;
}

const char* Function_application_t::arr_name(int i){
	return Param_arrays[i]->c_str();
}

bool Function_application_t::refer_to(std::string _fname){
	return _fname == Func_ref->name;
}

ISL::Set Function_application_t::get_isl_set(){
	return ISL::copy(set);
}

ISL::Set Function_application_t::get_named_isl_set(V_Input_t& Vin){
	return Func_ref->create_func_set(Vin);
}

void Function_application_t::name_dimension_isl_set(ISL::Set& set){
	Func_ref->name_dimension_isl_set(set);
}

void Function_application_t::name_isl_set(ISL::Set& set){
	Func_ref->name_isl_set(set);
}

Function_t* Function_application_t::get_ref(){
	return Func_ref;
}

size_t Function_application_t::nb_array() const{
	return Param_arrays.size();
}

std::string Function_application_t::get_arr_arg() const{
	std::string args;
	for(Array* arr : Param_arrays)
		args += arr->name + ", ";
	return args;
}

std::string Function_application_t::get_arr_arg(int i) const{
	return Param_arrays[i]->name;
}

Fill_argument_res Function_application_t::fill_argument(){
	Fill_argument_res far = Func_ref->fill_argument();
	oss arr_arg;
	/*Replace array names from function by array names from InKS code*/
	for(size_t i=0; i<Param_arrays.size(); ++i)
		arr_arg << "T" << i << "& " << Param_arrays[i]->name << ", ";
	far.arr_arg = arr_arg.str();
	return far;
}

size_t Function_application_t::get_array_index(std::string array_name){
	for(size_t i=0; i<Param_arrays.size(); ++i)
		if(array_name == Param_arrays[i]->name) return i;
	return -1;
}