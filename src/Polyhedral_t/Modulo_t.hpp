#ifndef MODULO_T_HPP_
#define MODULO_T_HPP_

#include <isl_cpp_type.hpp>
#include <Constraint_t.hpp>

class Expr_t;

struct Modulo_t : virtual public Constraint_t{	
	
	Constraint_t* c;
	
	Expr_t* min;
	
	int modulo;

	Modulo_t(Constraint_t* _c, Expr_t* _e, const char* _modulo);
	
	Modulo_t(Constraint_t* _c, Expr_t* _e, int _modulo);
	
	void init_lspace(ISL::LSpace _lspace);
	
	ISL::Constraint constraint_isl_alloc() const;
	
	bool is_modulo();
		
	// dim = stride * E
	void create_constraint(std::vector<ISL::Constraint>& v, int dim, TYPE_ISL type);

	void scope(Poly_scope_t& scope_object);
	
	Constraint_t* clone() const;
	
	virtual void print() const;
};

struct ModuloT_t : virtual public Constraint_t{	

	Constraint_t* c;
	
	int modulo;
	
	ModuloT_t(Constraint_t* _c, const char* _modulo);
	
	ModuloT_t(Constraint_t* _c, int _modulo);
	
	void init_lspace(ISL::LSpace _lspace);
	
	ISL::Constraint constraint_isl_alloc() const;
	
	bool is_modulo();
	
	// dim = stride * E
	void create_constraint(std::vector<ISL::Constraint>& v, int dim, TYPE_ISL type);
	
	void scope(Poly_scope_t& scope_object);
	
	Constraint_t* clone() const;
	
	void print() const;
};

#endif /* MODULO_T_HPP_ */
