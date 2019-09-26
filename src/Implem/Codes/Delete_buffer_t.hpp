#ifndef DELETE_BUFFER_T_HPP
#define DELETE_BUFFER_T_HPP

#include <string>

#include <Codes/Code_t.hpp>

class Delete_buffer_t : public Code_t{
	
	std::string id;
	
public:

	Delete_buffer_t(const std::string& _id);
	
	virtual std::string to_cpp_string() const;
};

#endif // DELETE_BUFFER_T_HPP
