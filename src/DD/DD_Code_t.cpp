#include <isl_processing.hpp>
#include <DD_Scope_t.hpp>
#include <isl_utils.hpp>
#include <DD_Code_t.hpp>
#include <DD_Line_t.hpp>
#include <DD_Gen_t.hpp>
#include <Implem_t.hpp>

DD_Code_t::DD_Code_t(DD_Line_t* _line){
	lines.push_back(_line);
}

DD_Code_t* DD_Code_t::push(DD_Line_t* _line){
	lines.push_back(_line);
	return this;
}

int DD_Code_t::nb_action() const{
	int act = 0;
	for(auto& line : lines)
		act += line->nb_action();
	return act;
}

void DD_Code_t::resolve(){
	std::vector<DD_Line_t*> new_lines;
	for(auto& line : lines)
		line->resolve(new_lines);
	lines = std::move(new_lines);
}

void DD_Code_t::check(const Time_Event_t& events){
	for(auto& line : lines)
		line->check(events);
}

void DD_Code_t::scope(DD_Scope_t& scope_object){
	for(auto& line : lines)
		line->scope(scope_object);
}
	
void DD_Code_t::generate(Implem_t& implem, const Time_Event_t& events){
	for(auto& line : lines)
		line->generate(implem, events, ISL::copy(valid_parameters));
}
	
void DD_Code_t::generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
	for(auto& line : lines)
		line->generate(implem, events, ISL::copy(context));
}
