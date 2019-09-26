#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>

#include <Poly_Exception_t.hpp>
#include <const_globals.hpp>
#include <Dependence_t.hpp>
#include <Poly_scope_t.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>
#include <utils.hpp>
#include <Array.hpp>

using std::string;
using std::cerr;
using std::cout;
using std::endl;

const std::string Array::COPY_NAME = "InKS_Copy_";
int Array::current_id = 0;

Array::Array(const char* _type, const char* _name, const char* _dim_size, Dependence_t* _io) :
	array_id(current_id++),
	type(_type),
	name(_name),
	io(_io),
	used(false),
	isl_wcondition(nullptr)
{
	if(_dim_size == nullptr)	dim_size = 0;
	else 						dim_size = (size_t)atoi(_dim_size);
}

Array::Array(const char* _type, const char* _name, const char* _dim_size) :
	Array(_type, _name, _dim_size, new Dependence_t())
{}

void Array::initDep(V_Input_t& Vin, List_var_t& vars){
	
	if ( verbosity ) cerr << "Initialize array: " << name << endl;
	
	Poly_scope_t scope_object(Vin);
	try{
		io->initDep(scope_object, vars, dim_size, isl_dim_out);
		
		//Check that input or output of the inks public kernel are bounded
		std::array<ISL::Map, 2> io_map {{io->getIn(), io->getOut()}};
		size_t unbounded_dim;
		for(ISL::Map m : io_map){
			if(! bounded_set(isl_map_range(m), unbounded_dim)){
				cerr << "***Error: Unbounded dimension(" << unbounded_dim << ")in array \"" +  name + "\"." << endl;
				exit(EXIT_FAILURE);
			}
		}	
	}catch(Ref_Exception_t& pe){
		cerr << "***Error: unknown variable \"" + pe.unknown_variable + "\" in array \"" +  name + "\"." << endl;
		exit(EXIT_FAILURE);
	}catch(Dependence_exception_t& de){
		cerr << "***Error: (" + de.dependence_type + " relation) sizes arent matching for the \"" + name + "\" array." << endl;
		exit(EXIT_FAILURE);
	}
	
	if ( verbosity ) cerr << endl;
}

bool Array::is_scalar() const{
	return dim_size == 0;
}

const char* Array::c_str(){
	return name.c_str();
}

ISL::Map Array::getOut(){
	string copy_name = Array::COPY_NAME + "out_" + name;
	ISL::Map r = isl_map_set_tuple_name(io->getOut(), isl_dim_in, copy_name.c_str());
	r = isl_map_set_tuple_name(r, isl_dim_out, name.c_str());
	return r;
}

ISL::Map Array::getIn(){
	string copy_name = Array::COPY_NAME + "in_" + name;
	ISL::Map r = isl_map_set_tuple_name(io->getIn(), isl_dim_in, copy_name.c_str());
	r = isl_map_set_tuple_name(r, isl_dim_out, name.c_str());
	return r;
}

ISL::Set Array::logical_set() const{
	return isl_set_set_tuple_name(universe_isl_set(dim_size), name.c_str());
}

ISL::Set Array::max_logical_set() const{
	return isl_set_set_tuple_name(ISL::copy(isl_set_max_value), name.c_str());
}

bool Array::is_IO() const{
	return io->has_IO() && used;
}

void Array::compute_valid_parameter(const V_Input_t& Vin, ISL::Set data){
	used = true;
	isl_set_max_value = isl_set_coalesce(ISL::copy(data));
	if(verbosity)
		ISL::print(isl_set_max_value, name + " - max cells ");
	isl_wcondition = isl_set_universe(Vin.param_space());
	for(size_t i=0; i<dim_size; i++){
		ISL::PWAff c_v = isl_pw_aff_coalesce(isl_set_dim_max(ISL::copy(data), i));
		isl_max_value.push_back(c_v);
		isl_wcondition = isl_set_coalesce(isl_set_intersect(isl_wcondition, isl_pw_aff_params(ISL::copy(c_v))));
	}
	
	if(verbosity) std::cout << name << " array gives the constraint -> " << ISL::str(isl_wcondition) << std::endl;
}

void Array::intersect_valid_parameter(ISL::Set valid_parameters){
	for(ISL::PWAff& pw : isl_max_value)
		pw = isl_pw_aff_coalesce(isl_pw_aff_gist_params(pw, ISL::copy(valid_parameters)));

	isl_set_max_value	= isl_set_coalesce(isl_set_gist_params(isl_set_max_value, ISL::copy(valid_parameters)));
	isl_wcondition		= isl_set_coalesce(isl_set_gist_params(isl_wcondition, ISL::copy(valid_parameters)));
}