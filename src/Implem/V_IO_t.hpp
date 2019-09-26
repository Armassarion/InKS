#ifndef V_BUFFER_T_HPP
#define V_BUFFER_T_HPP

#include <vector>
#include <string>
#include <array>

#include <IO_t.hpp>

struct io_auto_str{
	std::string tmpl;
	std::string param;
};

class V_IO_t{
	
	std::vector<IO_t> vio;
	
public:

	io_auto_str to_auto_string();
	
	std::string to_cpp_string();
	
	void push(IO_t&& io);
		
	size_t size() const;

};

#endif // V_BUFFER_T_HPP
