#ifndef DD_GEN_T_HPP
#define DD_GEN_T_HPP

#include <isl_cpp_type.hpp>

struct DD_Gen_t{
	
	ISL::USet data_gen;
	
	ISL::USet data_read;
	
	DD_Gen_t();

//Move
	DD_Gen_t(DD_Gen_t&&) = default;
	DD_Gen_t& operator=(DD_Gen_t&&) = default;
	
//Copy
	DD_Gen_t(const DD_Gen_t&) = delete;
	DD_Gen_t& operator=(const DD_Gen_t&) = delete;
	
	DD_Gen_t(ISL::USet _data_gen, ISL::USet _data_read);
	
	DD_Gen_t& operator+=(const DD_Gen_t& src);
	
	void print() const;

};


#endif