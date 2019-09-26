#ifndef DD_OMP_PARALLEL_REGION_T_HPP
#define DD_OMP_PARALLEL_REGION_T_HPP

#include <iostream>
#include <string>

#include <Codes/OMP_Parallel_Region_t.hpp>
#include <Scope_exception_t.hpp>
#include <isl_cpp_type.hpp>
#include <DD_Code_t.hpp>
#include <DD_Line_t.hpp>
#include <DD_Gen_t.hpp>
#include <Implem_t.hpp>

class List_t;

class DD_OMP_Parallel_Region_t : public DD_Line_t{
	
	List_t* privates;
	
	DD_Code_t* inner;
	
public:

	DD_OMP_Parallel_Region_t(int _lineno, List_t* _privates, DD_Code_t* _inner) : 
		DD_Line_t(_lineno),
		privates(_privates),
		inner(_inner)
	{}

	void scope(DD_Scope_t& scope_object){
		inner->scope(scope_object);
	}
	
	void generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
		Implem_t inner_implem;
		inner->generate(inner_implem, events, ISL::copy(context));
		
		implem.push_decl(inner_implem.declarations);
		implem.push_code(new OMP_Parallel_Region_t(privates, inner_implem.get_code_cpp_string()));
	}
	
	virtual void check(const Time_Event_t& events){}

	void resolve(std::vector<DD_Line_t*>& new_lines){
		DD_Line_t::resolve(new_lines);
		inner->resolve();
	}
};

#endif // DD_OMP_PARALLEL_REGION_T_HPP
