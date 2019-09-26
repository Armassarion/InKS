#ifndef FUNCTION_MAP_HPP_
#define FUNCTION_MAP_HPP_

#include <unordered_map>
#include <string>
#include <vector>

#include <Function_t.hpp>

class Fill_argument_res;
class V_Dependence_t;
class List_var_t;
class V_Input_t;
class Include_t;
class List_t;


struct Function_Map{
	
	using F_iterator = std::unordered_map<std::string, Function_t>::iterator;

	std::unordered_map<std::string, Function_t> Funcs;

	void push(char* _key, List_t* _dimension, V_Dependence_t* _vio, char* _code);
	
	void push(std::string _key, List_t* _dimension, V_Dependence_t* _vio, std::string _code);

	void add_application(char* _id_kernel, Function_application_t* _fa);

	size_t size();
	
	ISL::UMap getIn();
	
	ISL::UMap getOut();
	
	ISL::UMap getIn(const List_t& Fnames);
	
	ISL::UMap getOut(const List_t& Fnames);
	
	ISL::UMap getIn_Applied();
	
	ISL::UMap getOut_Applied();
	
	ISL::UMap getIn_Applied(const List_t& Fnames);
	
	ISL::UMap getOut_Applied(const List_t& Fnames);

	Function_t& at(std::string s);
	
	Function_t& at_no_error(std::string s);

	Function_t& operator[](std::string s);

	F_iterator begin();

	F_iterator end();
	
	F_iterator find(std::string);

	void init (V_Input_t&, List_var_t& vars);
	
	Fill_argument_res fill_argument(std::string);
	
	std::string get_declarations();
	
	std::string get_definitions();
	
	void gen_hpp_file(const Include_t& Include, const std::string& filename);
};


#endif /* FUNCTION_MAP_HPP_ */
