#ifndef LIST_VAR_T_HPP
#define LIST_VAR_T_HPP

#include <List_t.hpp>
#include <vector>
#include <string>

class Set_Poly_t;
class Var_t;

struct List_var_t{
	
	std::vector<Var_t*> ranges;
	
	List_var_t() 
	{}
	
	List_var_t(Var_t*);
	
	List_var_t* push(Var_t*);
	
	Var_t* get_var_poly();
	
	Set_Poly_t* get_poly(Set_Poly_t* p);
	
	List_t get_var_names() const;
	
	size_t size() const;
	
	Var_t* get(int i);
	
	std::string get_var_name(int i) const;
	
	void merge(List_var_t& _vars);
	
	void print() const;

	std::vector<Var_t*>::iterator begin();

	std::vector<Var_t*>::iterator end();
};

#endif