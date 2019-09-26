#ifndef FOR_T_HPP
#define FOR_T_HPP

#include <Codes/Code_t.hpp>
#include <isl_cpp_type.hpp>

class For_t : public Code_t{

	std::string iterator_name;
	
	ISL::Ast_expr min;
	
	ISL::Ast_expr cond;
	
	ISL::Ast_expr inc;
	
	Code_t* inner;
	
	Code_t* before_outside;
	
	Code_t* before_inside;
	
	Code_t* after_outside;
	
	Code_t* after_inside;
	
public:

	For_t(const std::string& _iterator, ISL::Ast_expr _min, ISL::Ast_expr _cond, ISL::Ast_expr _inc);
	
	void set_inner(Code_t* c);
	
	void set_before_outside(Code_t* c);
	
	void set_before_inside(Code_t* c);
	
	void set_after_outside(Code_t* c);
	
	void set_after_inside(Code_t* c);
	
	virtual std::string to_cpp_string() const;

};

#endif // FOR_T_HPP
