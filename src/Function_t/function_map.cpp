#include <iostream>
#include <sstream>
#include <fstream>

#include <V_Dependence_t.hpp>
#include <const_globals.hpp>
#include <Function_Map.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>
#include <List_t.hpp>
#include <utils.hpp>
#include <Include_t.hpp>

using std::out_of_range;
using std::string;
using std::endl;

void Function_Map::push(char* _key, List_t* _dimension, V_Dependence_t* _vio, char* _code){
	push(string(_key), _dimension, _vio, string(_code));
}

void Function_Map::push(string _key, List_t* _dimension, V_Dependence_t* _vio, string _code){
	Funcs[_key] = Function_t(_key, _dimension, _vio, Funcs.size(), _code);
}

void Function_Map::add_application(char* _id_kernel, Function_application_t* _fa){
	string key(_id_kernel);
	try{
		Funcs.at(key).add_application(_fa);
	}catch (const std::out_of_range& oor){
		std::cerr << "Out of Range error: " << oor.what() << '\n';
	}
}

size_t Function_Map::size(){
	return Funcs.size();
}

ISL::UMap Function_Map::getIn(){
	List_t Fnames;
	for(const auto& Func : Funcs)
		Fnames.push(Func.first);
	return this->getIn(Fnames);
}

ISL::UMap Function_Map::getIn(const List_t& Fnames){
	ISL::UMap r = empty_isl_union_map();
	for(const auto& fname : Fnames)
		r = isl_union_map_union(this->at(fname).getIn(), r);
	return r;
}

ISL::UMap Function_Map::getOut(){
	List_t Fnames;
	for(const auto& Func : Funcs)
		Fnames.push(Func.first);
	return this->getOut(Fnames);
}

ISL::UMap Function_Map::getOut(const List_t& Fnames){
	ISL::UMap r = empty_isl_union_map();
	for(const auto& fname : Fnames)
		r = isl_union_map_union(this->at(fname).getOut(), r);
	return r;
}

ISL::UMap Function_Map::getIn_Applied(){
	ISL::UMap r = empty_isl_union_map();
	for(auto& f : Funcs)
		r = isl_union_map_union(f.second.getIn_Applied(), r);
	return r;
}

ISL::UMap Function_Map::getIn_Applied(const List_t& Fnames){
	ISL::UMap r = empty_isl_union_map();
	for(const auto& fname : Fnames)
		r = isl_union_map_union(this->at(fname).getIn_Applied(), r);
	return r;
}

ISL::UMap Function_Map::getOut_Applied(){
	ISL::UMap r = empty_isl_union_map();
	for(auto& f : Funcs)
		r = isl_union_map_union(f.second.getOut_Applied(), r);
	return r;
}

ISL::UMap Function_Map::getOut_Applied(const List_t& Fnames){
	ISL::UMap r = empty_isl_union_map();
	for(const auto& fname : Fnames)
		r = isl_union_map_union(this->at(fname).getOut_Applied(), r);
	return r;
}

Function_t& Function_Map::at(string s){
	try{
		return Funcs.at(s);
	}catch(const out_of_range& oor){
		std::cerr << "Out of Range error: " << oor.what() << " (function key " << s << ").\n";
		std::cerr << endl << "Known functions: " << endl;
		for(auto& f : Funcs)
			std::cerr << f.second.name << endl;
		exit(EXIT_FAILURE);
	}
}

Function_t& Function_Map::at_no_error(string s){
	return Funcs.at(s);
}

Function_t& Function_Map::operator[](string s){
	return Funcs[s];
}

Function_Map::F_iterator Function_Map::begin(){
	return Funcs.begin();
}

Function_Map::F_iterator Function_Map::end(){
	return Funcs.end();
}

Function_Map::F_iterator Function_Map::find(std::string name){
	return Funcs.find(name);
}

void Function_Map::init(V_Input_t& Vin, List_var_t& vars){
	for(auto& f : Funcs)
		f.second.init(Vin, vars);
}

Fill_argument_res Function_Map::fill_argument(std::string key){
	return at(key).fill_argument();
}

std::string Function_Map::get_declarations(){
	std::ostringstream ret;
	
	for(const auto& f : Funcs)
		ret << f.second.get_prototype(Function_t::SEMI_COLUMN) << endl << endl;
	ret << std::endl;
	
	return ret.str();
}

std::string Function_Map::get_definitions(){
	std::ostringstream ret;
	
	for(const auto& f : Funcs){
		ret << f.second.get_prototype(Function_t::NO_SEMI_COLUMN) << endl;
		ret << f.second.get_code();
	}
	
	return ret.str();
}

void Function_Map::gen_hpp_file(const Include_t& Include, const std::string& filename){
	std::ofstream hppfile(filename);
	
	string include_guard = generate_include_guard(filename);
	hppfile << "#ifndef " << include_guard << endl;
	hppfile << "#define " << include_guard << endl;
	hppfile << Include.to_string() << endl;
	hppfile << this->get_declarations() << endl;
	hppfile << this->get_definitions() << endl;
	hppfile << "#endif" << endl;		
	hppfile.close();
}