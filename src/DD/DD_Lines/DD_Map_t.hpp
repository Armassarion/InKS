#ifndef DD_MAP_T_HPP
#define DD_MAP_T_HPP

#include <string>
#include <vector>
#include <map>

#include <isl_cpp_type.hpp>
#include <DD_Line_t.hpp>

class DD_Scope_t;
class Set_Poly_t;
class List_var_t;
class Implem_t;
class DD_Gen_t;
class List_t;

class DD_Map_t : public DD_Line_t{
		
	List_var_t* vars;
	
	std::string physical_id;
	
	Set_Poly_t* physical_set;
	
	std::string logical_id;
	
	Set_Poly_t* logical_set; 
		
	ISL::Map map;
	
public:

	DD_Map_t(int _lineno, List_var_t* _rl, const char* _pid, Set_Poly_t* _ps, const char* _lid, Set_Poly_t* _ls);
	
	DD_Map_t(int _lineno, List_var_t* _rl, const char* _pid, const char* _lid, Set_Poly_t* _ls);
		
	DD_Map_t(int _lineno, const char* _pid, Set_Poly_t* _ps, const char* _lid, Set_Poly_t* _ls);
	
	DD_Map_t(int _lineno, const char* _pid, const char* _lid, Set_Poly_t* _ls);
	
	DD_Map_t(int _lineno, const char* _pid, const char* _lid);
	
	int nb_action() const {return 2;}
	
	void check(const Time_Event_t& events);
	
	virtual void scope(DD_Scope_t& scope_object);
	
	virtual void generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context);
	
	ISL::Map compute_map(DD_Scope_t& scope_object);
		
	ISL::Map get_map() const;
	
	std::string get_physical_id() const;
	
	std::string get_logical_id() const;
	
	size_t get_physical_dim_size() const;
	
	Set_Poly_t* get_physical_set() const;
};


#endif // DD_MAP_T_HPP
