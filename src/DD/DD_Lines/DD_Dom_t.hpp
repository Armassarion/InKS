#ifndef DD_DOM_HPP
#define DD_DOM_HPP

#include <string>

#include <DD_Line_t.hpp>

class List_var_t;
class DD_Gen_t;
class Var_t;

class DD_Dom_t : public DD_Line_t{

private :

	List_var_t* vars;
	
public:
	
	DD_Dom_t(int _lineno, List_var_t* _v) : 
		DD_Line_t(_lineno),
		vars(_v)
	{}
	
	void generate(Implem_t&, const Time_Event_t& events, ISL::Set context) {}
	
	void check(const Time_Event_t& events) {}
	
	void scope(DD_Scope_t& scope_object){
		for(Var_t* v : *vars)
			scope_object.add_var(v);
	}
	
};

#endif // DD_DOM_HPP
