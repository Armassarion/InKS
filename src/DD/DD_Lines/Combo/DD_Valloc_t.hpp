#ifndef DD_VALLOC_T_HPP
#define DD_VALLOC_T_HPP

#include <Combo/DD_Combo_t.hpp>
#include <DD_Decl_t.hpp>
#include <DD_Mem_t.hpp>

class DD_Valloc_t : public DD_Combo_t{
	
	DD_Mem_t* alloc;
	
public:

	DD_Valloc_t(DD_Mem_t* _alloc) : 
		alloc(_alloc)
	{}
	
	virtual void resolve(std::vector<DD_Line_t*>& new_lines){
		new_lines.push_back(new DD_Decl_t(alloc->get_lineno(), alloc->get_type(), alloc->get_physical_id(), alloc->get_dim_size()));
		new_lines.push_back(alloc);
	}

};

#endif
