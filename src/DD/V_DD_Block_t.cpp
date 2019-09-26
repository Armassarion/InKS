#include <iostream>
#include <sstream>

#include <V_DD_Block_t.hpp>
#include <DD_Block_t.hpp>
#include <Generate_t.hpp>
#include <Implem_t.hpp>

V_DD_Block_t::V_DD_Block_t(DD_Block_t* _block){
	blocks.push_back(_block);
}
	
V_DD_Block_t* V_DD_Block_t::push(DD_Block_t* _block){
	blocks.push_back(_block);
	return this;
}

void V_DD_Block_t::resolve(){
	for(auto& block : blocks)
		block->resolve();
}

void V_DD_Block_t::scope(ISL::Set valid_parameters, Generate_t& Gen){
	for(auto& block : blocks){
		if(block->is_the_chosen_one(Gen.name, Gen.Vin))
			block->scope(valid_parameters, Gen.Arrays, isl_union_map_range(Gen.createInputMap()));
		else
			block->scope(valid_parameters, Gen.Arrays);
	}
}

std::vector<Implem_t> V_DD_Block_t::generate(){
	std::vector<Implem_t> implems;
	
	for(auto& block : blocks)
		implems.emplace_back(std::move(block->generate()));

	return implems;
}