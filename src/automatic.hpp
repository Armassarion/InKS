#ifndef AUTOMATIC_GENERATION__
#define AUTOMATIC_GENERATION__

#include <string>

#include <isl_cpp_type.hpp>

class Function_Map;
class Generate_t;

namespace Auto{
	void parsing(std::string);
	void choices_generation(std::string, Function_Map&, Generate_t&);
}

#endif
