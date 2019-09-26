#ifndef DD_PARAM_T_HPP
#define DD_PARAM_T_HPP

#include <iostream>

#include <DD_Mem_t.hpp>

class DD_Scope_t;
class DD_Map_t;
class DD_Gen_t;

struct IOMap_t{
	DD_Map_t* in;
	
	DD_Map_t* out;
	
	IOMap_t(DD_Map_t*, DD_Map_t*);
};

class DD_Param_t : public DD_Mem_t{
	
	IOMap_t* io_maps;
	
public:
	
	DD_Param_t(int _lineno, OWN _prop, const char * _type, const char* _pid, Set_Poly_t* _sizes, IOMap_t* _io);
	
	DD_Param_t(int _lineno, OWN _prop, const char * _type, const char* _pid, IOMap_t* _io);
	
	void check(const Time_Event_t& events);
	
	void scope(DD_Scope_t& scope_object);

	void generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context);
	
};

#endif // DD_PARAM_T_HPP
