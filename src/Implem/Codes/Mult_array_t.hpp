#ifndef MULT_ARRAY_T_HPP
#define MULT_ARRAY_T_HPP

#include <string>

#include <isl_cpp_type.hpp>
#include <Codes/Code_t.hpp>

class Mult_array_t : public Code_t{
	
	std::string name;
	
	ISL::Set sizes_isl;
	
public:

	Mult_array_t(const std::string& _n, ISL::Set _s);
		
	virtual std::string to_cpp_string() const;

};

#endif // MULT_ARRAY_T_HPP
