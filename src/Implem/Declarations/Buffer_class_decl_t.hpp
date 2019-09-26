#ifndef BUFFER_DECL_T_HPP
#define BUFFER_DECL_T_HPP

#include <string>

#include <Declarations/Declaration_t.hpp>
#include <isl_cpp_type.hpp>
#include <Input_t.hpp>

class Buffer_class_decl_t : public Declaration_t{
	
	std::string logical_name;
	
	std::string type;
	
	uint id;
	
	V_Input_t Vin;
	
	ISL::UMap mapping;
	
	std::string const_attr() const;
		
	std::string const_setter() const;
	
	std::string const_param() const;
	
	std::string ptr_attr() const;
	
	std::string ptr_setter(ISL::L_Map) const;
	
	std::string ptr_param(ISL::L_Map) const;
	
	std::string parenthesis_operator() const;
	
	std::string constructor() const;
	
public:

	Buffer_class_decl_t(const std::string& _s, const std::string& _t, uint _id, const V_Input_t& _v, ISL::UMap _m);
	
	Buffer_class_decl_t(const std::string& _s, const std::string& _t, const V_Input_t& _v, ISL::UMap _m);
	
	std::string to_cpp_decl() const;
};

#endif // BUFFER_DECL_T_HPP
