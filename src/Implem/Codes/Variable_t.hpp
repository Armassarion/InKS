#ifndef VARIABLE_T_HPP
#define VARIABLE_T_HPP

#include <string>

#include <Codes/Code_t.hpp>

class Variable_t : public Code_t{
	
	std::string id;
	
	std::string type;
	
	int dim;
	
public:

	Variable_t(const std::string& _id, const std::string& _type, int _dim);
	
	virtual std::string to_cpp_string() const;
};

#endif // VARIABLE_T_HPP
