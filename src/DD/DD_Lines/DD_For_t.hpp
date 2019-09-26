#ifndef DD_FOR_T_HPP
#define DD_FOR_T_HPP

#include <string>
#include <isl_cpp_type.hpp>
#include <DD_Line_t.hpp>
#include <DD_Gen_t.hpp>
#include <Implem_t.hpp>

class List_named_poly_t;
class V_DDForNamedVar;
class DDForOptions_t;
class Time_Event_t;
class DD_Scope_t;
class DD_Code_t;
class DD_Gen_t;


class DD_For_t : public DD_Line_t{
			
	List_named_poly_t* data_out;
	
	V_DDForNamedVar* order;
	
	DD_Code_t* inner;
	
	ISL::USet data_gen;
	
	std::string gen_code;
	
	ISL::UMap RO_Inner_rel;
	
	ISL::UMap RI_Inner_rel;
	
	DD_Gen_t inner_gen;
	
	ISL::Map order_map;
	
	ISL::Map order_map_param;
			
	void schedule(DD_Scope_t& scope_object);
	
	//Return a map from order to data
	ISL::UMap getRel(ISL::USet data) const;
	
	ISL::Set get_iteration_context();
	
public:

	DD_For_t() = delete;

	DD_For_t(int _lineno, List_named_poly_t* _data_out, V_DDForNamedVar* _order, DD_Code_t* _inner);
	
	int nb_action() const {return 1;}
	
	void generate(Implem_t&, const Time_Event_t& events, ISL::Set context);
	
	void check(const Time_Event_t& events);
	
	void scope(DD_Scope_t& scope_object);
	
	virtual void resolve(std::vector<DD_Line_t*>& new_lines);
	
};


#endif // DD_FOR_T_HPP