#ifndef V_DD_BLOCK_T_HPP
#define V_DD_BLOCK_T_HPP

#include <isl_cpp_type.hpp>
#include <vector>

class Generate_t;
class DD_Block_t;
class Implem_t;

class V_DD_Block_t{
	
	std::vector<DD_Block_t*> blocks;
	
public:

	V_DD_Block_t() = default ;
	
	V_DD_Block_t(DD_Block_t*);
	
	V_DD_Block_t* push(DD_Block_t*);
	
	void resolve();
	
	void scope(ISL::Set valid_parameters, Generate_t& Gen);
	
	std::vector<Implem_t> generate();
};

#endif // V_DD_BLOCK_T_HPP
