#ifndef FUNCTION_DECL_T_HPP
#define FUNCTION_DECL_T_HPP

#include <string>
#include <vector>

#include <Declarations/Declaration_t.hpp>

class Function_Decl_t : public Declaration_t{
	
	std::vector<std::string> templates;
	
	std::string return_type;
	
	std::string name;
	
	std::string prototype;
	
	std::string body;
	
public:

	Function_Decl_t(std::vector<std::string>&& _t, const std::string& _rtype, const std::string& _name, const std::string& _prototype, const std::string& _body);
	
	std::string to_cpp_decl() const;
	
};

#endif // FUNCTION_T_HPP
