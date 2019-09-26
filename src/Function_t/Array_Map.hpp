#ifndef ARRAY_MAP_HPP_
#define ARRAY_MAP_HPP_

#include <string>
#include <map>

#include <isl_cpp_type.hpp>
#include <Array.hpp>

class List_var_t;

struct Array_Map{
	
	std::map<std::string, Array*> arrs;
	
	Array_Map(Array* array);

	Array_Map* push(Array* array);
	
	size_t size(const std::string& key) const;

	size_t size() const;
	
	bool contains(const std::string& key) const;

	Array& at(const std::string& key);
	
	const Array& at(const std::string& key)const;
	
	Array* get_ptr(const std::string& key);

	std::map<std::string, Array*>::iterator begin();
	
	std::map<std::string, Array*>::iterator end();

	void initDep(V_Input_t& Vin, List_var_t& vars);

	bool is_IO(const std::string& key) const;

	ISL::UMap createInputMap();

	ISL::UMap createOutputMap();

	//set representing the union of the logical arrays sets
	ISL::USet logical_sets() const;
	
	ISL::USet max_logical_sets() const;

	ISL::Set valid_parameter(const V_Input_t& Vin, ISL::USet data_used);
		
	void intersect_valid_parameter(ISL::Set valid_parameters);
};

#endif /* ARRAY_MAP_HPP_ */
