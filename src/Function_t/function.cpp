#include <iostream>
#include <sstream>
#include <string>

#include <Function_application_t.hpp>
#include <Poly_Exception_t.hpp>
#include <V_Dependence_t.hpp>
#include <const_globals.hpp>
#include <Poly_scope_t.hpp>
#include <Dependence_t.hpp>
#include <Function_t.hpp>
#include <Array_Map.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>
#include <List_t.hpp>
#include <utils.hpp>
#include <Array.hpp>

using oss = std::ostringstream;
using std::to_string;
using std::string;
using std::cerr;
using std::endl;

Function_t::Function_t()
{}

Function_t::Function_t(string _name, List_t* _dimension, V_Dependence_t* _vio, int _id, string _code):
		id(_id),
		name(_name),
		dimension(_dimension),
		vio(_vio),
		code(_code)
{
	Fill_argument_res far = fill_argument();
	oss ssproto, sscode;
	size_t found = code.find("#END");
	if(found != std::string::npos){
		code.erase(found);
		code.erase(0, code.find_first_of("\n")+1);
		
		sscode << "{\n" << code << "}\n\n";
		code = sscode.str();
		
		ssproto << far.tmpl << "\nvoid " << name;
		ssproto << "(" << far.arr_arg + far.struct_arg << ")";
		prototype = ssproto.str();
	}
}

Function_t::Function_t(char* _name, List_t* _dimension, V_Dependence_t* _vio, int _id, char* _code)
	:	Function_t(string(_name), _dimension, _vio, _id, string(_code)) {}
	
size_t Function_t::dim_size(){
	return dimension->size();
}

size_t Function_t::nb_array(){
	return vio->size();
}

size_t Function_t::nb_param(){
	return dimension->size() + vio->size();
}

void Function_t::add_application(Function_application_t* _fa){
	vfa.push(_fa);
}

ISL::Map Function_t::getIn(size_t i){
	return vio->getIn(i);
}

ISL::Map Function_t::getOut(size_t i){
	return vio->getOut(i);
}

ISL::UMap Function_t::getIn(){
	return vio->getIn(name.c_str());
}

ISL::UMap Function_t::getOut(){
	return vio->getOut(name.c_str());
}

ISL::UMap Function_t::getIn_Applied(){
	return vfa.get_InRelation(Function_application_t::Instance_name_mode::INSTANCE);
}

ISL::UMap Function_t::getOut_Applied(){
	return vfa.get_OutRelation(Function_application_t::Instance_name_mode::INSTANCE);
}

ISL::Set Function_t::create_func_set(V_Input_t& Vin){
	return dimension->create_dimension_set(Vin);
}

void Function_t::init(V_Input_t& Vin, List_var_t& vars){
	if (verbosity) cerr << "--------> Init function: " << name << endl;
	try{
		Poly_scope_t scope_object(Vin, dimension);
		vio->initDep(scope_object, vars, dimension->size(), isl_dim_in);
	}catch(Ref_Exception_t& pe){
		cerr << "***Error: unknown variable \"" + pe.unknown_variable + "\" in kernel \"" +  name + "\"." << endl;
		exit(EXIT_FAILURE);
	}catch(Dependence_exception_t& de){
		cerr << "***Error: (" + de.dependence_type + " relation) sizes arent matching in the " + name + " kernel for the " << de.name << " array." << endl;
		exit(EXIT_FAILURE);
	}
	if (verbosity) cerr << endl;
}

int Function_t::get_id(){
	return id;
}

ISL::Set Function_t::name_dimension_isl_set(ISL::Set& s){
	return dimension->name_isl_set(s);
}

ISL::Set Function_t::name_isl_set(ISL::Set& s){
	return isl_set_set_tuple_name(dimension->name_isl_set(s), name.c_str());
}

Fill_argument_res Function_t::fill_argument(){
	string dim = dimension->fill_param();
	string tmpl = fill_template();
	string arr = fill_arr_arg();
	if(dim.empty()) delete_last(arr, 2);
	return Fill_argument_res{tmpl, arr, dim};
}

std::string Function_t::fill_template(){
	oss tmpl;
	tmpl << "template <";
	for(size_t i=0; i<vio->size(); i++)
		tmpl << "typename T" << i << add_coma_if(i, vio->size());
	tmpl << ">";
	return tmpl.str();
}

std::string Function_t::fill_arr_arg(){
	oss arr_arg;
	for(size_t i=0; i<vio->size(); i++)
		arr_arg << "T" << i << "& " << vio->get_arr_name(i) << ", ";
	return arr_arg.str();
}

std::string Function_t::get_arr_arg(){
	return vio->get_arr_name();
}

std::string Function_t::get_arr_arg(int i){
	return vio->get_arr_name(i);
}

std::string Function_t::get_prototype(COLUMN clmn) const{
	return prototype + (clmn == SEMI_COLUMN ? ";" : "");
}

std::string Function_t::get_code() const{
	return code;
}

Dependence_t* Function_t::get_dependence_from(std::string array_name){
	try{
		return vio->get(vfa.get_array_index(array_name));
	}catch(const std::out_of_range& oor){
		std::cerr << "***Error: no " << name << " application found that uses the " << array_name << " array." << std::endl;
		exit(EXIT_FAILURE);
	}
}

bool Function_t::is_self_dependent(size_t considered_dim){
	ISL::UMap R = isl_union_map_coalesce(isl_union_map_apply_range(getOut(), isl_union_map_reverse(getIn())));
	ISL::BMap bR;
	if(isl_union_map_is_empty(R) == isl_bool_true) //No dependences between each kernel call
		return false;
		
	auto lambda_union_to_basic_map = [](ISL::Map m, void* user) -> isl_stat{
		if(isl_map_n_basic_map(m) != 1) return isl_stat_error;
	
		auto lambda_map_to_basic = [](ISL::BMap m, void* user) -> isl_stat{
			ISL::BMap* bR = (ISL::BMap*)user;
			*bR = m;
			return isl_stat_ok;
		};
		
		return isl_map_foreach_basic_map(m, lambda_map_to_basic, user);
	};	
	if(		isl_union_map_n_map(R) != 1
		|| 	isl_union_map_foreach_map(R, lambda_union_to_basic_map, &bR) == isl_stat_error
	)
		return false;
	
	ISL::Mat mat = isl_basic_map_equalities_matrix(bR, isl_dim_in, isl_dim_out, isl_dim_cst, isl_dim_param, isl_dim_div);
	size_t n_cols = (dimension->size() * 2) + 1; //in + out + cst
	size_t dim_out = considered_dim + dimension->size();
	size_t dim_in = considered_dim;
	
	for(int row=0; row<isl_mat_rows(mat); row++){
		if(has_constraint(mat, row, dim_in, dim_out)){
			for(size_t cols=0; cols<n_cols; cols++){
				//if different de "cols_in == cols_out"
				if(cols != dim_in && cols != dim_out && elem_is_not_zero(mat, row, cols))
					return true;
			}
		}
	}

	return false;
}