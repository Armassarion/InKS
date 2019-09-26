#ifndef DD_BLOCK_T_HPP
#define DD_BLOCK_T_HPP

#include <vector>
#include <string>

#include <isl_cpp_type.hpp>
#include <Time_Event_t.hpp>
#include <DD_Scope_t.hpp>

class DD_Param_t;
class V_Input_t;
class DD_Code_t;
class Array_Map;
class Implem_t;
class List_t;

class DD_Alloc_param_t{
	
	std::vector<DD_Param_t*> alloc;

public:

	DD_Alloc_param_t(){};
	
	DD_Alloc_param_t(DD_Param_t* _d);
	
	DD_Alloc_param_t* push(DD_Param_t* _d);
	
	void add_to_scope(DD_Scope_t&);
	
	void generate_io(Implem_t& implem, const Time_Event_t& events);
	
	std::string get_alloc_params() const;
};

class DD_Block_t{
	
	std::string id;
	
	DD_Alloc_param_t* alloc_params;
	
	V_Input_t const_params;
	
	DD_Code_t* inner;
	
	DD_Scope_t my_scope;
	
	Time_Event_t events;
	
public:

	DD_Block_t(const char* _id, DD_Alloc_param_t* _allocs, List_t* _const, DD_Code_t* _inner);
	
	DD_Block_t(const char* _id, DD_Alloc_param_t* _allocs, DD_Code_t* _inner);
	
	DD_Block_t(const char* _id, List_t* _const, DD_Code_t* _inner);
	
	void resolve();
	
	void scope(ISL::Set valid_parameters, Array_Map*, ISL::USet data_in);
	
	void scope(ISL::Set valid_parameters, Array_Map*);
	
	Implem_t generate();
		
	bool is_the_chosen_one(const std::string& public_id, const V_Input_t& public_param) const;
	
};

#endif // DD_BLOCK_T_HPP
