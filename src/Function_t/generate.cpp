#include <iostream>
#include <fstream>
#include <sstream>

#include <V_Function_application_t.hpp>
#include <Function_application_t.hpp>
#include <const_globals.hpp>
#include <Function_Map.hpp>
#include <List_var_t.hpp>
#include <Generate_t.hpp>
#include <Array_Map.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>
#include <List_t.hpp>
#include <Array.hpp>
#include <utils.hpp>

using oss = std::ostringstream;
using std::ofstream;
using std::string;
using std::vector;
using std::cerr;
using std::endl;

void Generate_t::init(const char* _name, List_t* _param, Array_Map* _Arrays, V_Function_application_t* _Apply, const char* _restrict){
	Vin.init(_param);
	Arrays	= _Arrays;
	Apply  	= _Apply;
	name   	= string(_name);
	restrict= string(_restrict);

	trim(restrict);
	replace_all(restrict, "#RESTRICT");
}

void Generate_t::scope(Function_Map& Functions){
	Apply->scope(Functions, *Arrays);
}

void Generate_t::add_vars(List_var_t* _vars){
	vars.merge(*_vars);
}

void Generate_t::initDep(){
	return Arrays->initDep(Vin, vars);
}

ISL::Set Generate_t::get_restrict() const{
	oss restrict_str;
	restrict_str << "[";
	for(uint i=0; i<Vin.size(); i++)
		restrict_str << Vin.get_name(i) << add_coma_if(i, Vin.size());
	restrict_str << "] -> { : " << restrict << "}";
	
	return isl_set_coalesce(isl_set_read_from_str(ctx, restrict_str.str().c_str()));
}
	
ISL::UMap Generate_t::createOutputMap(){
	return Arrays->createOutputMap();
}
	
ISL::UMap Generate_t::createInputMap(){
	return Arrays->createInputMap();
}

bool Generate_t::no_generate(){
	if(Apply == nullptr || Apply->size() == 0)
		return true;
	return false;
}

ISL::USet Generate_t::createApplySet(){
	return isl_union_set_coalesce(Apply->createApplySet(Vin, vars));
}

ISL::UMap Generate_t::createInRelation(){
	return isl_union_map_coalesce(Apply->get_InRelation(Function_application_t::Instance_name_mode::INSTANCE));
}

ISL::UMap Generate_t::createOutRelation(){
	return isl_union_map_coalesce(Apply->get_OutRelation(Function_application_t::Instance_name_mode::INSTANCE));
}

std::string Generate_t::get_params(bool b){
	return Vin.get_params(b);
}

ISL::Set Generate_t::array_isl_property(ISL::USet& SI, ISL::USet& SO){
	ISL::USet data_used = isl_union_set_coalesce(isl_union_set_union(ISL::copy(SI), ISL::copy(SO)));
	ISL::Set valid_parameters = isl_set_coalesce(Arrays->valid_parameter(Vin, data_used));
	bool read_not_written = false;
	SI = isl_union_set_coalesce(isl_union_set_intersect_params(SI, ISL::copy(valid_parameters)));
	SO = isl_union_set_coalesce(isl_union_set_intersect_params(SO, ISL::copy(valid_parameters)));
	
	read_not_written = (isl_union_set_is_subset(SI, SO) == isl_bool_false);
	if(read_not_written){
		ISL::USet diffSISO = isl_union_set_coalesce(isl_union_set_subtract(ISL::copy(SI), ISL::copy(SO)));
		ISL::Set param_constraints = isl_set_empty(Vin.param_space());
		
		
		isl_union_set_foreach_set(
			diffSISO, 
			[] (ISL::Set set, void* user) -> isl_stat {
				ISL::Set* param_constraints = (ISL::Set*)user;
				*param_constraints = isl_set_union(*param_constraints, isl_set_params(set));
				return isl_stat_ok;
			},
			&param_constraints
		);
		param_constraints = isl_set_coalesce(isl_set_complement(param_constraints));
		valid_parameters = isl_set_coalesce(isl_set_intersect_params(param_constraints, valid_parameters));
		SI = isl_union_set_coalesce(isl_union_set_intersect_params(SI, ISL::copy(valid_parameters)));
		SO = isl_union_set_coalesce(isl_union_set_intersect_params(SO, ISL::copy(valid_parameters)));
		read_not_written = (isl_union_set_is_subset(SI, SO) == isl_bool_false);
	}

	if(read_not_written){
		cerr << "*** ERROR: some data are read but never written." << endl;
		if(!verbosity){
			cerr << "* Run with \"-v\" for more details." << endl;
		}else{
			cerr << "The \"Restricted input\" set is not a subset of the \"Restricted output\" set. Differences:" << endl;
			cerr << isl_union_set_to_str(isl_union_set_coalesce(isl_union_set_subtract(ISL::copy(SI), ISL::copy(SO)))) << endl << endl;
		}
		exit(EXIT_FAILURE);
	}
	
	return isl_set_coalesce(valid_parameters);
}

Function_application_t* Generate_t::fa_from_instance_name(const string& instance_name){
	return Apply->find_from_instance_name(instance_name);
}

Function_application_t* Generate_t::find_from_func_name(const std::string& _fname){
	return Apply->find_from_func_name(_fname);
}
