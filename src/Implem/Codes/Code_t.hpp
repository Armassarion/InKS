#ifndef CODE_T_HPP
#define CODE_T_HPP

#include <iostream>
#include <string>

class Code_t{
	
public:
	
	Code_t(){}
	
	virtual std::string to_cpp_string() const = 0;
};

class Str_code_t : public Code_t{
	
	std::string code;

public:
	
	Str_code_t(const std::string& _s) :
		code(_s)
	{}
		
	virtual std::string to_cpp_string() const{
		return code;
	}
};

#endif // CODE_T_HPP
