#ifndef DD_COMBO_T_HPP
#define DD_COMBO_T_HPP

#include <iostream>

#include <Scope_exception_t.hpp>
#include <DD_Line_t.hpp>
#include <DD_Gen_t.hpp>

class DD_Combo_t : public DD_Line_t{
	
public:

	virtual void resolve(std::vector<DD_Line_t*>& new_lines) = 0;
	
	void check(const Time_Event_t& events){
		throw Scope_exception_t("Combo shouldn't have a check");
	}
	
	void scope(DD_Scope_t& scope_object){
		throw Scope_exception_t("Combo shouldn't have a scope");
	}

	void generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
		throw Scope_exception_t("Combo can't generate anything");
	}
	
};

#endif
