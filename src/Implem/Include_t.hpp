#ifndef CODE_T_HPP_
#define CODE_T_HPP_

#include <string>

class Include_t{
	
	std::string include;
	
public :

	Include_t();
	
	void push(const char* _input);

	std::string to_string() const;

};


#endif /* CODE_T_HPP_ */
