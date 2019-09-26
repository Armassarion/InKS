#ifndef DD_SCOPE_EXCEPTION_T_HPP
#define DD_SCOPE_EXCEPTION_T_HPP

#include <exception>
#include <string>

struct Scope_exception_t : public std::exception{
	std::string message;
	
	Scope_exception_t(int _lineno, const std::string& s){
		message = std::string("Line ") + std::to_string(_lineno) + ": " + s;
	}
	
	Scope_exception_t(const std::string& s) : message(s){}
	
	const char* what() const noexcept {
		return message.c_str();
	}
	
};

#endif