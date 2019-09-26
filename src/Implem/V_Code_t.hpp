#ifndef V_CODE_T_HPP
#define V_CODE_T_HPP

#include <list>

#include <Codes/Code_t.hpp>

class V_Code_t{
	
	std::list<Code_t*> codes;
	
public:

	void push(Code_t*);
	
	void push(const std::string& _s);
	
	void push_front(Code_t*);
		
	std::string to_cpp_string() const;
};

#endif // V_CODE_T_HPP
