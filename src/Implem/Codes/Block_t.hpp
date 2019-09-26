#ifndef BLOCK_T_HPP
#define BLOCK_T_HPP

#include <Codes/Code_t.hpp>

class Block_t : public Code_t{

protected:
	
	const Code_t* inner;
	
public:

	Block_t(const Code_t* _c);
	
	virtual std::string to_cpp_string() const;

};

#endif // BLOCK_T_HPP
