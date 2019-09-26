#include <sstream>
#include <vector>

#include <V_Dependence_t.hpp>
#include <const_globals.hpp>
#include <Dependence_t.hpp>
#include <Poly_scope_t.hpp>
#include <Array.hpp>

using oss = std::ostringstream;
using std::string;
using std::vector;

V_Dependence_exception_t::V_Dependence_exception_t(Dependence_exception_t& _de, int _n) :
	Dependence_exception_t(_de),
	n(_n)
{}

V_Dependence_t::V_Dependence_t(Dependence_t* _d){
	vio.push_back(_d);
}

V_Dependence_t* V_Dependence_t::push(Dependence_t* _d){
	vio.push_back(_d);
	return this;
}

void V_Dependence_t::initDep(Poly_scope_t& scope_object, List_var_t& vars, size_t dim_size, isl_dim_type type){
	for(Dependence_t* dep : vio)
		dep->initDep(scope_object, vars, dim_size, type);
}

Dependence_t* V_Dependence_t::get(size_t i){
	return vio[i];
}

ISL::Map V_Dependence_t::getIn(size_t i){
	return vio[i]->getIn();
}

ISL::Map V_Dependence_t::getOut(size_t i){
	return vio[i]->getOut();
}

ISL::UMap V_Dependence_t::getIn(const char* in_name=""){
	ISL::UMap r = isl_union_map_empty(isl_space_alloc(ctx, 0, 0, 0));
	for(Dependence_t* d : vio){
		ISL::Map Rin = isl_map_set_tuple_name(d->getIn(), isl_dim_out, d->get_arr_name().c_str());
		Rin = isl_map_set_tuple_name(Rin, isl_dim_in, in_name);
		r = isl_union_map_union(isl_union_map_from_map(Rin), r);
	}
	return r;
}

ISL::UMap V_Dependence_t::getOut(const char* in_name=""){
	ISL::UMap r = isl_union_map_empty(isl_space_alloc(ctx, 0, 0, 0));
	for(Dependence_t* d : vio){
		ISL::Map Rout = isl_map_set_tuple_name(d->getOut(), isl_dim_out, d->get_arr_name().c_str());
		Rout = isl_map_set_tuple_name(Rout, isl_dim_in, in_name);
		r = isl_union_map_union(isl_union_map_from_map(Rout), r);
	}
	return r;
}

size_t V_Dependence_t::size(){
	return vio.size();
}

string V_Dependence_t::get_arr_name(size_t i){
	return vio[i]->get_arr_name();
}

string V_Dependence_t::get_arr_name(){
	oss r;
	for(Dependence_t* d : vio)
		r << d->get_arr_name() << ", ";
	return r.str();
}

void V_Dependence_t::alter_arr_name(string new_name, string ex_name){
	for(Dependence_t* d : vio){
		if(d->arr_name == ex_name){
			d->arr_name = new_name;
			break;
		}
	}
}

void V_Dependence_t::restore_arr_name(string new_name, string ex_name){
	for(Dependence_t* d : vio)
		if(d->arr_name == new_name){
			d->arr_name = ex_name;
			break;
		}
}

std::vector<Dependence_t*>::iterator V_Dependence_t::begin(){
	return vio.begin();
}

std::vector<Dependence_t*>::iterator V_Dependence_t::end(){
	return vio.end();
}

void V_Dependence_t::check_arrays_dimsize(vector<Array*>& Arrays){
	size_t i;
	try{
		for(i=0; i<size(); i++)
			vio[i]->check_dim_size(Arrays[i]->dim_size);
	}catch(Dependence_exception_t& de){
		throw V_Dependence_exception_t(de, i);
	}
}