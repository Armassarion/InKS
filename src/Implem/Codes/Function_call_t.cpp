#include <Codes/Function_call_t.hpp>
#include <const_globals.hpp>
#include <utils.hpp>

Function_call_t::Function_call_t(const std::string& _n, std::vector<std::string>&& _p) : 
	name(_n),
	params(_p)
{}

std::string Function_call_t::to_cpp_string() const{
	std::string inlining(inline_mode ? "#pragma forceinline recursive\n" : "");
	std::string str_params;
	for(size_t i=0; i<params.size(); i++)
		str_params += params[i] + add_coma_if(i, params.size());
		
	return inlining + name + "(" + str_params + ");\n";
}