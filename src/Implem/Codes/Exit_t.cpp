#include <sstream>

#include <Codes/Exit_t.hpp>

Exit_t::Exit_t(const std::string& _message) :
	message(_message)
{}

std::string Exit_t::to_cpp_string() const{
	std::ostringstream r;
	r << "std::cerr << \"" << message << "\" << std::endl;" << std::endl;;
	r << "exit(EXIT_SUCCESS);";
	return r.str();
}
