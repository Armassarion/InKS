#ifndef DECLARATION_T_HPP
#define DECLARATION_T_HPP

class Declaration_t{
	
	std::string decl;
	
public :
	
	Declaration_t(){}
	
	Declaration_t(const std::string& _d) :
		decl(_d)
	{}
	
	virtual std::string to_cpp_decl() const{
		return decl;
	}

};

#endif // DECLARATION_T_HPP
