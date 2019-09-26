#ifndef FUNCTION_CALL_T_HPP
#define FUNCTION_CALL_T_HPP

#include <Codes/Code_t.hpp>

#include <string>
#include <vector>

class Function_call_t : public Code_t{
	
	std::string name;
	
	std::vector<std::string> params;
	
public:

	Function_call_t(const std::string& _n, std::vector<std::string>&& _p);
	
	virtual std::string to_cpp_string() const;
};

#endif // FUNCTION_CALL_T_HPP
