#ifndef EXIT_T_HPP
#define EXIT_T_HPP

#include <sstream>

#include <Codes/Code_t.hpp>

class Exit_t : public Code_t{
	
	std::string message;
	
public:

	Exit_t(const std::string& _message="");

	virtual std::string to_cpp_string() const;
};

#endif // EXIT_T_HPP
