#include <stdexcept>
#include <iostream>
#include <sstream>
#include <tuple>
#include <map>

#include <const_globals.hpp>
#include <Dependence_t.hpp>
#include <Array_Map.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>
#include <utils.hpp>

using oss = std::ostringstream;
using std::out_of_range;
using std::string;
using std::endl;
using std::map;

Array_Map::Array_Map(Array* array){
	push(array);
}

Array_Map* Array_Map::push(Array* array){
	arrs[array->name] = array;
	//std::cout << "Array #" << array->array_id << ": " << array->name << std::endl;
	return this;
}

size_t Array_Map::size(const string& key) const {
	return this->at(key).dim_size;
}

size_t Array_Map::size() const{
	return arrs.size();
}

bool Array_Map::contains(const std::string& key) const{
	return arrs.count(key);
}

Array& Array_Map::at(const string& key){
	try {
		return *(arrs.at(key));
	} catch(out_of_range& oor) {
		std::cerr << "Out of Range error: " << oor.what() << " (array key \"" << key << "\").\n";
		std::cerr << endl << "Known arrays: " << endl;
		for(auto& a : arrs)
			std::cerr << a.second->name << endl;
		exit(EXIT_FAILURE);
	}
}

const Array& Array_Map::at(const string& key) const{
	try {
		return *(arrs.at(key));
	} catch(out_of_range& oor) {
		std::cerr << "Out of Range error: " << oor.what() << " (array key " << key << ").\n";
		std::cerr << endl << "Known arrays: " << endl;
		for(auto& a : arrs)
			std::cerr << a.second->name << endl;
		exit(EXIT_FAILURE);
	}
}

Array* Array_Map::get_ptr(const std::string& key){
	return & (this->at(key));
}

map<string, Array*>::iterator Array_Map::begin(){
	return arrs.begin();
}
map<string, Array*>::iterator Array_Map::end(){
	return arrs.end();
}

void Array_Map::initDep(V_Input_t& Vin, List_var_t& vars){
	for(auto& A : arrs)
		A.second->initDep(Vin, vars);
}

bool Array_Map::is_IO(const string& key) const{
	return this->at(key).is_IO();
}

ISL::UMap Array_Map::createInputMap(){
	ISL::UMap r = empty_isl_union_map();
	for(auto& arr : arrs)
		r = isl_union_map_union(r, isl_union_map_from_map(arr.second->getIn()));
	return isl_union_map_coalesce(r);
}

ISL::UMap Array_Map::createOutputMap(){
	ISL::UMap r = empty_isl_union_map();
	for(auto& arr : arrs)
		r = isl_union_map_union(r, isl_union_map_from_map(arr.second->getOut()));
	return isl_union_map_coalesce(r);
}

ISL::USet Array_Map::logical_sets() const{
	ISL::USet r = empty_isl_union_set();
	for(const auto& arr : arrs)
		r = isl_union_set_union(r, isl_union_set_from_set(arr.second->logical_set()));
	return r;
}

ISL::USet Array_Map::max_logical_sets() const{
	ISL::USet r = empty_isl_union_set();
	for(const auto& arr : arrs)
		r = isl_union_set_union(r, isl_union_set_from_set(arr.second->max_logical_set()));
	return r;
}

ISL::Set Array_Map::valid_parameter(const V_Input_t& Vin, ISL::USet data_used){
	ISL::Set r = isl_set_universe(Vin.param_space());
	
	ISL::L_Set lset = isl_union_set_get_set_list(data_used);
	int nb = isl_set_list_size(lset);
	for(int i=0; i<nb; i++){
		ISL::Set data = isl_set_list_get_set(lset, i);
		Array& A = this->at(isl_set_get_tuple_name(data));
		A.compute_valid_parameter(Vin, data);
		r = isl_set_intersect(r, ISL::copy(A.isl_wcondition));
	}
	
	return isl_set_coalesce(r);
}

void Array_Map::intersect_valid_parameter(ISL::Set valid_parameters){
	for(auto& a : arrs)
		a.second->intersect_valid_parameter(valid_parameters);
}