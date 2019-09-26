#ifndef REFERENCE_T_HPP_
#define REFERENCE_T_HPP_

#include <isl_cpp_type.hpp>
#include <Expr_t.hpp>
#include <string.h>

class Poly_scope_t;

struct Reference_t : public Expr_t{
	const char* name;
	
	int pos;
	
	Reference_t(const char* _name, int _pos = -1) : 
		name(_name),
		pos(_pos)
	{}

	virtual int fill_constraint(ISL::Constraint& c, isl_dim_type type, int coef);

	Expr_t* scope(Poly_scope_t& scope_object);
	
	virtual Expr_t* clone() const;
	
	void print() const;
};

struct Constante_t : public Reference_t{

	Constante_t(const char* _name, int _pos) : 
		Reference_t(_name, _pos)
	{}

	int fill_constraint(ISL::Constraint& c, isl_dim_type type, int coef);
	
	virtual Expr_t* clone() const;
};

#endif /* REFERENCE_T_HPP_ */
