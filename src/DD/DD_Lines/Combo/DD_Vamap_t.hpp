#ifndef DD_VAMAP_T_HPP
#define DD_VAMAP_T_HPP

#include <Combo/DD_Combo_t.hpp>
#include <DD_Decl_t.hpp>
#include <DD_Mem_t.hpp>
#include <DD_Map_t.hpp>

class DD_Vamap_t : public DD_Combo_t{
	
	std::string type;
	
	DD_Map_t* map;
	
public:

	DD_Vamap_t(const char* _type, DD_Map_t* _map) : 
		type(_type),
		map(_map)
	{}
	
	virtual void resolve(std::vector<DD_Line_t*>& new_lines){
		int dim_size = map->get_physical_dim_size();
		new_lines.push_back(new DD_Decl_t(map->get_lineno(), type, map->get_physical_id(), dim_size));
		
		if(dim_size == 0)
			new_lines.push_back(new DD_Mem_t(map->get_lineno(), type, map->get_physical_id()));
		else
			new_lines.push_back(new DD_Mem_t(map->get_lineno(), type, map->get_physical_id(), map->get_physical_set(), DD_Mem_t::OWN::Take, true));
			
		new_lines.push_back(map);
	}

};

#endif
