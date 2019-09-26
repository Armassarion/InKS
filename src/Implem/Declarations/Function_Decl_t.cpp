#include <Declarations/Function_Decl_t.hpp>
#include <utils.hpp>

Function_Decl_t::Function_Decl_t(std::vector<std::string>&& _t, const std::string& _rtype, const std::string& _name, const std::string& _prototype, const std::string& _body) : 
	templates(_t),
	return_type(_rtype),
	name(_name),
	prototype(_prototype),
	body(_body)
{}

std::string Function_Decl_t::to_cpp_decl() const{
	std::string r;
	if(!templates.empty()){
		r = "template<";
		for(size_t i=0; i<templates.size(); ++i)
			r += "typename " + templates[i] + add_coma_if(i, templates.size());
		r += ">\n";
	}
	
	return r + return_type + " " + name + prototype + "\n{\n" + body + "}\n";
}