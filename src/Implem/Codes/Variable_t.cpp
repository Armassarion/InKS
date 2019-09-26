#include <Codes/Variable_t.hpp>

Variable_t::Variable_t(const std::string& _id, const std::string& _type, int _dim) : 
	id(_id),
	type(_type),
	dim(_dim)
{}

std::string Variable_t::to_cpp_string() const{
	if(dim > 0)
		return type + "* " + id + ";\n";
	else
		return type + " " + id + ";\n";
}