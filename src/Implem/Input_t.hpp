#ifndef INKS_INPUT_HPP__
#define INKS_INPUT_HPP__

#include <vector>
#include <string>

#include <isl_cpp_type.hpp>

class List_t;

struct Input_t{	
	
	std::string name;
	
	ISL::Id id;
	
	Input_t(std::string _name);
	
	Input_t(const char* _name);
	
	Input_t(Input_t&& i);
	
	Input_t(const Input_t& i);
	
	Input_t& operator=(const Input_t&);
	
	Input_t();
	
	~Input_t();
};

class V_Input_t{	
	
	std::vector<Input_t> vin;
	
	const Input_t& struct_input(size_t i) const ;
	
	ISL::Id get_isl_id(size_t i) const ;
	
	ISL::Space& fill_input_id(ISL::Space&) const;
	
public:
	
	V_Input_t();
	
	V_Input_t(const V_Input_t&);
	
	V_Input_t& merge(const V_Input_t& v);
	
	V_Input_t& merge(const List_t* v);
	
	V_Input_t(const List_t* l);
		
	V_Input_t& push(const char* _name);
	
	void init(const List_t* l);
	
	V_Input_t& operator=(const V_Input_t&);
	
	bool operator==(const V_Input_t&) const;
	
	Input_t& operator[](int i);
	
	const Input_t& operator[](int i) const;
	
	std::string get_name(size_t i) const;
	
	size_t size() const;
			
	ISL::Space map_space(int dim_in, int dim_out) const;
	
	ISL::Space set_space(int dim) const;
	
	ISL::Space param_space() const;
	
	std::string get_params(bool type=false) const;
	
	int get_constant_pos(const char* _name) const;

	std::vector<Input_t>::iterator begin();
	
	std::vector<Input_t>::iterator end();
	
	const std::vector<Input_t>::const_iterator begin() const;
	
	const std::vector<Input_t>::const_iterator end() const;
	
	void print() const;
};

#endif
