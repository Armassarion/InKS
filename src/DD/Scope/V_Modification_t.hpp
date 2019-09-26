#ifndef V_MODIFICATION_T_HPP
#define V_MODIFICATION_T_HPP

#include <Modification_t.hpp>
#include <isl_cpp_type.hpp>
#include <Time_Event_t.hpp>
#include <vector>
#include <memory>

class Time_Event_t;
class List_t;

class V_Modification_t{
	
	std::vector<std::unique_ptr<Modification_t>> modifs;
	
	//[time .... all to 0]
	ISL::Set get_original_sch(int& sch_size) const;
		
	int sch_size() const;

public:
	
	void push_map(DD_Line_t* _line, ISL::Map map);
	
	void push_unmap(DD_Line_t* _line, ISL::Set unmap);
	
	void push_read(DD_Line_t* _line, ISL::USet read);
	
	void push_write(DD_Line_t* _line, ISL::USet write);
	
	void push_readphy(DD_Line_t* _line, ISL::USet read);
	
	void push_writephy(DD_Line_t* _line, ISL::USet write);
	
	void operator+=(V_Modification_t& vm);
	
	Time_type_t& push_addfor(DD_Line_t* _line, ISL::Set addfor, int nb_action);
	
	void push_delfor(DD_Line_t* _line, ISL::Set delfor, Time_type_t& t);
		
	Time_Event_t get_SchEvent(ISL::Set context, ISL::USet logical_areas);
	
	void print() const;
	
	std::vector<std::unique_ptr<Modification_t>>::iterator begin();
	
	std::vector<std::unique_ptr<Modification_t>>::iterator end();
};

#endif // V_MODIFICATION_T_HPP
