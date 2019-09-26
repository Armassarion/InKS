#include <sstream>

#include <V_Code_t.hpp>

void V_Code_t::push(const std::string& _s){
	codes.push_back(new Str_code_t(_s));
}

void V_Code_t::push(Code_t* c){
	codes.push_back(c);
}

void V_Code_t::push_front(Code_t* c){
	codes.push_front(c);
}

std::string V_Code_t::to_cpp_string() const{
	std::ostringstream r;
	for(const Code_t* c : codes){
		std::string cpp_code(std::move(c->to_cpp_string()));
		r << (cpp_code.empty() ? "" : cpp_code + "\n");
	}
	
	return r.str();
}

