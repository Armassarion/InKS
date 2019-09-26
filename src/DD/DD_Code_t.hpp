#ifndef DD_CODE_T_HPP
#define DD_CODE_T_HPP

#include <vector>

#include <isl_cpp_type.hpp>

class Time_Event_t;
class DD_Scope_t;
class DD_Line_t;
class DD_Gen_t;
class Implem_t;

class DD_Code_t{
	
	std::vector<DD_Line_t*> lines;
		
public:

	DD_Code_t(DD_Line_t*);
	
	DD_Code_t* push(DD_Line_t*);
	
	int nb_action() const;

	void resolve();
	
	void check(const Time_Event_t& events);
	
	void scope(DD_Scope_t& scope_object);
	
	void generate(Implem_t&, const Time_Event_t& events);
	
	void generate(Implem_t&, const Time_Event_t& events, ISL::Set context);

};

#endif // DD_CODE_T_HPP
