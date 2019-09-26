#include <iostream>
#include <string>

#include <const_globals.hpp>
#include <Dependence_t.hpp>
#include <Poly_scope_t.hpp>
#include <Set_Poly_t.hpp>
#include <List_var_t.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>
#include <List_t.hpp>

using std::string;
using std::endl;
using std::cerr;

Dependence_t::Dependence_t(Set_Poly_t* _in, Set_Poly_t* _out) : 
	set_in(_in), 
	set_out(_out),
	arr_dimsize(0),
	arr_name("")
{
	if(set_in != nullptr) 		arr_dimsize = set_in->dim_size();
	else if(set_out != nullptr) arr_dimsize = set_out->dim_size();
}

Dependence_t::Dependence_t(string _arr_name) : 
	set_in(nullptr), 
	set_out(nullptr), 
	arr_dimsize(0), 
	arr_name(_arr_name)
{}

Dependence_t::Dependence_t() : 
	Dependence_t("")
{}


Dependence_t* Dependence_t::add_arr_info(const char* _arr_name, const char* _arr_type){
	arr_name = _arr_name;
	arr_type = _arr_type;
	return this;
}

void Dependence_t::initDep(Poly_scope_t scope_object, List_var_t& vars, size_t dim_size, isl_dim_type type){
	if(set_in == nullptr)
		Rin = isl_map_empty(scope_object.vin.map_space(dim_size, 0));
	else{
		Rin = createDepMap(scope_object, vars, set_in);
		if(isl_map_dim(Rin, type) != dim_size) throw Dependence_exception_t(arr_name, "input");
	}
	
	if(set_out == nullptr)
		Rout = isl_map_empty(scope_object.vin.map_space(dim_size, 0));
	else{
		Rout = createDepMap(scope_object, vars, set_out);
		if(isl_map_dim(Rout, type) != dim_size) throw Dependence_exception_t(arr_name, "output");
	}
	
	if ( verbosity ) {
		cerr << "In " << arr_name << ") " << isl_map_to_str(Rin) << endl;
		cerr << "Out " << arr_name << ") " << isl_map_to_str(Rout) << endl;
	}
}

ISL::Map Dependence_t::createDepMap(Poly_scope_t& scope_object, List_var_t& vars, Set_Poly_t* set){
	ISL::Set func_set = scope_object.create_func_set();
	ISL::Map map = isl_map_coalesce(vars.get_poly(set)->createMap(scope_object, func_set));
	name_map_in_from_set(map, func_set);
	return map;
}

ISL::Map Dependence_t::getIn(){
	return ISL::copy(Rin);
}

ISL::Map Dependence_t::getOut(){
	return ISL::copy(Rout);
}

string Dependence_t::get_arr_name(){
	return arr_name;
}

bool Dependence_t::has_IO(){
	return set_in != nullptr || set_out != nullptr;
}

void Dependence_t::check_dim_size(size_t dim_size){
	if(set_in != nullptr && dim_size != set_in->dim_size())
		throw Dependence_exception_t(arr_name, "input");
	
	if(set_out != nullptr && dim_size != set_out->dim_size())
		throw Dependence_exception_t(arr_name, "output");
}