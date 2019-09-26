#ifndef DD_FREE_T_HPP
#define DD_FREE_T_HPP

#include <iostream>
#include <string>

#include <Codes/Delete_buffer_t.hpp>
#include <Scope_exception_t.hpp>
#include <DD_Line_t.hpp>
#include <DD_Gen_t.hpp>
#include <Implem_t.hpp>

class DD_Free_t : public DD_Line_t{
	
	std::string array_id;
	
public:

	DD_Free_t(int _lineno, const char* _pid) : 
		DD_Line_t(_lineno),
		array_id(_pid)
	{}
	
	int nb_action() const {return 1;}

	void check(const Time_Event_t& events){}
	
	void scope(DD_Scope_t& scope_object){
		my_scope = scope_object;
		
		if(!scope_object.alloc_is_not_const(array_id))
			throw Scope_exception_t(lineno, array_id + " cannot be freed because it isn't allocated or must be kept\n");
		
		ISL::Set area = universe_isl_set(scope_object.decl_dim_size(array_id));
		area = isl_set_set_tuple_name(area, array_id.c_str());
		
		scope_object.modifications.push_unmap(this, area);
		scope_object.sub_mem(array_id);
	}	
	
	void generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
		implem.push_code(new Delete_buffer_t(array_id));
	}
};

#endif // DD_UPDATE_T_HPP
