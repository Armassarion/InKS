#ifndef CONSTRAINT_T_HPP_
#define CONSTRAINT_T_HPP_

#include <isl_cpp_type.hpp>
#include <isl_utils.hpp>
#include <vector>
#include <string>

class Poly_scope_t;

struct Constraint_t{
	
	enum TYPE_ISL {In, Out, Set, Param}; //vital because isl_dim_set == isl_dim_out...
	
	ISL::LSpace lspace;
	
	Constraint_t() : 
		lspace(nullptr)
	{}
	
	isl_dim_type get_reverse_type_isl(TYPE_ISL ti){
		return (ti == Set) ? isl_dim_set : ((ti == Param) ? isl_dim_in : ((ti == In) ? isl_dim_out : isl_dim_in));
	}
	
	isl_dim_type get_type_isl(TYPE_ISL ti){
		return (ti == Set) ? isl_dim_set : ((ti == Param) ? isl_dim_param : ((ti == In) ? isl_dim_in : isl_dim_out));
	}
	
	
	virtual ISL::Constraint constraint_isl_alloc() const = 0;
	virtual void create_constraint(std::vector<ISL::Constraint>& v, int dim, TYPE_ISL type) = 0;
	virtual void scope(Poly_scope_t& scope_object) = 0;
	virtual Constraint_t* clone() const = 0;
	virtual void print() const = 0;
	
	virtual void init_lspace(ISL::LSpace _lspace){ lspace = ISL::copy(_lspace); }
	virtual Constraint_t* unset_supremum(){return this;};
	virtual bool is_modulo(){return false;};
};


#endif /* CONSTRAINT_T_HPP_ */
