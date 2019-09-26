#ifndef LOGTOPHY_ARRAY_T_HPP
#define LOGTOPHY_ARRAY_T_HPP

#include <string>

#include <isl_cpp_type.hpp>
#include <Codes/Code_t.hpp>
#include <Input_t.hpp>

class Buffer_class_t : public Code_t{
	
	std::string lid;
	
	std::string pid;
	
	ISL::UMap memmap;
	
	const V_Input_t& csts;
	
public:

	Buffer_class_t(const std::string& _lid, ISL::UMap _memmap, const V_Input_t& _csts);
	
	virtual std::string to_cpp_string() const;
};

#endif // LOGTOPHY_ARRAY_T_HPP
