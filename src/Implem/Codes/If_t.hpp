#ifndef IF_T_HPP
#define IF_T_HPP

#include <isl_cpp_type.hpp>
#include <Codes/Code_t.hpp>
#include <Codes/Block_t.hpp>

class If_t : public Block_t{

protected:
	
	ISL::Set context;
	
	bool neg;
	
public:
	
	If_t(const Code_t* _c, ISL::Set _context, bool negate=false);
	
	virtual std::string to_cpp_string() const;

};

#endif // IF_T_HPP
