#ifndef V_DECLARATION_T_HPP
#define V_DECLARATION_T_HPP

#include <vector>
#include <string>

#include <Declarations/Declaration_t.hpp>

class V_Declaration_t{
	
	std::vector<Declaration_t*> vdecl;
	
public:

	void push(Declaration_t*);
	
	void push(const std::string& _s);
	
	std::string to_cpp_decl() const;
	
	void merge(V_Declaration_t&);
	
	std::vector<Declaration_t*>::iterator begin();
	
	std::vector<Declaration_t*>::iterator end();
	
	const std::vector<Declaration_t*>::const_iterator begin() const;
	
	const std::vector<Declaration_t*>::const_iterator end() const;

};

#endif // V_DECLARATION_T_HPP
