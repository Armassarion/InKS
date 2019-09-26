#ifndef BUFFER_T_HPP
#define BUFFER_T_HPP

#include <string>

#include <isl_cpp_type.hpp>
#include <Codes/Code_t.hpp>

struct Buffer_t : public Code_t{

	Buffer_t(const std::string& _n, const std::string& _t, ISL::Set _s, int _di, int _do);
	
	Buffer_t(const std::string& _n, const std::string& _t, ISL::Set _s, int _do);
	
	std::string name;
	
	std::string type;
	
	ISL::Set sizes_isl;
	
	int dim_in;
	
	int dim_out;
		
	virtual std::string to_cpp_string() const;
	
};

#endif // BUFFER_T_HPP
