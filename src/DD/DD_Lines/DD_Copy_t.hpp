#ifndef DD_COPY_T_HPP
#define DD_COPY_T_HPP

#include <isl_cpp_type.hpp>
#include <DD_Line_t.hpp>
#include <string>

class List_named_poly_t;
class DD_Gen_t;

class DD_Copy_t : public DD_Line_t{
	
	static uint copy_id;
	
	List_named_poly_t* set;
	
	std::string pid;
	
	ISL::USet area;
	
	ISL::UMap Wmap;
	
	ISL::UMap Rmap;
	
public:

	DD_Copy_t(int line, List_named_poly_t* _set);
	
	int nb_action() const {return 1;}
	
	void check(const Time_Event_t& events);
	
	void scope(DD_Scope_t& scope_object);
	
	void generate(Implem_t&, const Time_Event_t& events, ISL::Set context);
	
};

#endif // DD_COPY_T_HPP
