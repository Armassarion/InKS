#ifndef POLY_EXCEPTION_T_HPP
#define POLY_EXCEPTION_T_HPP

#include <stdexcept>
#include <iostream>
#include <string>

struct Ref_Exception_t : public std::invalid_argument{
	std::string unknown_variable;
	
	Ref_Exception_t(const char* _uv) : 
		std::invalid_argument::invalid_argument(std::string("Unknown variable: ") + std::string(_uv)),
		unknown_variable(_uv)
	{}
};


#endif // POLY_EXCEPTION_T_HPP
