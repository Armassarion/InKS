#ifndef TIME_EVENT_T_HPP
#define TIME_EVENT_T_HPP

#include <Modification_t.hpp>
#include <isl_cpp_type.hpp>
#include <map>

class Time_Event_t{
	
	ISL::UMap read;
	
	ISL::UMap write;
	
	ISL::UMap maps;
		
	std::map<std::string, ISL::Set> logical_array_max;
	
	void gist_logical_area(ISL::UMap& umap) const;
	
public:
	
	Time_Event_t();
	
	Time_Event_t(int sch_size, int max_time, ISL::UMap time_to_map, ISL::UMap time_to_unmap, ISL::UMap time_to_read, ISL::UMap time_to_write, ISL::UMap time_to_readphy, ISL::UMap time_to_writephy, ISL::USet logical_areas, ISL::Set context);
	
	ISL::UMap map_from_time_to_write(ISL::Set sch) const;
	
	ISL::UMap map_from_time_to_read(ISL::Set sch) const;
	
	void print() const;
};

#endif // TIME_EVENT_T_HPP
