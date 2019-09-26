#ifndef DD_GENERATION__
#define DD_GENERATION__

#include <string>

#include <isl_cpp_type.hpp>

class Function_Map;
class V_DD_Block_t;
class Generate_t;

namespace DD{
	void parsing(std::string, V_DD_Block_t& DD_Blocks, Generate_t& Gen);
	std::string choices_generation(V_DD_Block_t&, Function_Map&, Generate_t&);
	void parse_and_generate(std::string, std::string, std::string, Function_Map&, Generate_t&);
}

#endif
