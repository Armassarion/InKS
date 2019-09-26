#ifndef LIST_CONSTRAINTS_T_HPP
#define LIST_CONSTRAINTS_T_HPP

#include <vector>

#include <isl_cpp_type.hpp>
#include <Constraint_t.hpp>

class List_constraints_t : virtual public Constraint_t {
	
	std::vector<Constraint_t*> constraints;
	
public:

	List_constraints_t();
	
	List_constraints_t(Constraint_t* _c);
	
	List_constraints_t* push(Constraint_t* _c);
	
	virtual ISL::Constraint constraint_isl_alloc() const {};
	
	virtual void create_constraint(std::vector<ISL::Constraint>& v, int dim, TYPE_ISL type);
	
	virtual void scope(Poly_scope_t& scope_object);
	
	virtual Constraint_t* clone() const;

	virtual void init_lspace(ISL::LSpace _lspace);
	
	virtual bool is_modulo();
	
	virtual void print() const;

};

#endif // LIST_CONSTRAINTS_T_HPP
