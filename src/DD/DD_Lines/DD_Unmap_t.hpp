#ifndef DD_UNMAP_T_HPP
#define DD_UNMAP_T_HPP

#include <DD_Line_t.hpp>
#include <string>

class Set_Poly_t;
class DD_Gen_t;

class DD_Unmap_t : public DD_Line_t{
	
	std::string pid;
	
	Set_Poly_t* set;
	
public:

	DD_Unmap_t(int line, const char* _pid, Set_Poly_t* _set = nullptr);
	
	int nb_action() const {return 1;}
	
	void check(const Time_Event_t& events);
	
	void scope(DD_Scope_t& scope_object);
	
	void generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context);
	
};

#endif // DD_UNMAP_T_HPP
