#ifndef IO_T_HPP
#define IO_T_HPP

#include <string>

class IO_t{

public:
		
	IO_t(std::string _n, std::string _t, uint _d, bool _is_const=false);
	
	IO_t(IO_t&& io);
	
	std::string name;
	
	std::string type;
	
	uint dims;
	
	bool is_const;
		
	std::string to_auto_string(uint i);
	
	std::string to_cpp_string();
		
	IO_t& operator=(const IO_t& b);
};

#endif // BUFFER_T_HPP
