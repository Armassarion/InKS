#include <sstream>

#include <Codes/Block_t.hpp>

Block_t::Block_t(const Code_t* _c):
	inner(_c)
{}

std::string Block_t::to_cpp_string() const{
	std::ostringstream r;
	r << "{\n" << inner->to_cpp_string() << "\n}\n";
	return r.str();
}