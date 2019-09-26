#ifndef DD_UPDATE_T_HPP
#define DD_UPDATE_T_HPP

#include <string>
#include <vector>
#include <map>

#include <isl_cpp_type.hpp>
#include <DD_Line_t.hpp>

class List_named_poly_t;
class DD_Gen_t;

class DD_Update_t : public DD_Line_t{

protected :

	static std::map<std::string, uint> allocator_id;
	
	List_named_poly_t* parsed_area;
	
	List_t* kernel_ids;
	
	ISL::UMap RO, RI;
	
	ISL::USet SO, SI;
	
	ISL::USet Objective;
	
	virtual void print_kernel() const;

	void test(ISL::UMap Rw, ISL::UMap Rr, ISL::USet written, ISL::USet read, ISL::Set context);
		
	void generate_scheduling(Implem_t& implem, const Time_Event_t& events, ISL::Set context);
	
	void generate_mapping(Implem_t& implem, const Time_Event_t& events, ISL::Set context);
	
	void add_read_write(DD_Scope_t& scope_object);
	
public:

	DD_Update_t(int _lineno, List_named_poly_t* _up, List_t* _kids);
	
	DD_Update_t(int _lineno, List_named_poly_t* _up);
	
	int nb_action() const {return 2;}
	
	virtual void generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context);
	
	void check(const Time_Event_t& events);
	
	virtual void scope(DD_Scope_t& scope_object);
	
};

#endif // DD_UPDATE_T_HPP
