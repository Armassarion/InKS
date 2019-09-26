#ifndef DD_LINE_T_HPP
#define DD_LINE_T_HPP

#include <string>
#include <vector>
#include <map>

#include <Modification_t.hpp>
#include <isl_cpp_type.hpp>
#include <DD_Scope_t.hpp>

class DD_Gen_t;
class Implem_t;

class DD_Line_t{

public:

	DD_Scope_t my_scope;

	int lineno;
	
	std::map<Modification_t::Type, ISL::Set> time;
	
	DD_Line_t() : lineno(-1){}
	
	DD_Line_t(int _lineno) : lineno(_lineno) {}
	
	int get_lineno() const {return lineno;}
	
	void set_time(Modification_t::Type t, ISL::Set _time) {time[t] = isl_set_copy(_time);}
	
	virtual int nb_action() const {return 0;}
	
	virtual void resolve(std::vector<DD_Line_t*>& new_lines){
		new_lines.push_back(this);
	}
	
	virtual void check(const Time_Event_t& events) = 0;
	
	virtual void scope(DD_Scope_t& scope_object) = 0;
	
	virtual void generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context) = 0;
	
};

#endif // DD_LINE_T_HPP
