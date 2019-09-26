#ifndef POLY_SCOPE_T__
#define POLY_SCOPE_T__

#include <iostream>
#include <string>
#include <vector>

#include <isl_cpp_type.hpp>
#include <Input_t.hpp>
#include <List_t.hpp>

struct Poly_scope_t{
	
	V_Input_t vin;
	
	const List_t* func_set;
	
	List_t vars;
	
	Poly_scope_t(const V_Input_t& _vin, const List_t* _func_set) : 
		vin(_vin),
		func_set(_func_set)
	{}
	
	Poly_scope_t(const V_Input_t& _vin, ISL::Set _func_set) : 
		Poly_scope_t(_vin, new List_t(_func_set))
	{}
	
	Poly_scope_t(const V_Input_t& _vin) : 
		Poly_scope_t(_vin, new List_t())
	{}
	
	Poly_scope_t(const Poly_scope_t& _so) : 
		vin(_so.vin),
		func_set(new List_t(*(_so.func_set))),
		vars(_so.vars)
	{}
	
	ISL::Set create_func_set(){
		return func_set->create_dimension_set(vin);
	}
	
	ISL::Space param_space(){
		return vin.param_space();
	}
	
	ISL::Space set_space(int s=0){
		return vin.set_space(s);
	}
	
	ISL::Space map_space(int i=0, int o=0){
		return vin.map_space(i, o);
	}
	
	void add_var(std::string _n){
		vars.push(_n);
	}
	
	int pos_const(const char* _n){
		int r = vin.get_constant_pos(_n);
		if(r == -1){
			r = vars.pos(_n);
			return r == -1 ? r : (r + vin.size());
		}
		return r;
	}
	
	int pos_ref(const char* _n){
		return func_set->pos(_n);
	}
	
	void print() const {
		std::cout << "Const scope: " << std::endl;
		vin.print();
		std::cout << "Vars scope: " << std::endl;
		vars.print();
		std::cout << "Func scope: " << std::endl;
		func_set->print();
	}
};


#endif /* POLY_SCOPE_T__ */