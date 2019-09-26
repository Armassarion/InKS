#include <sstream>

#include <Codes/If_t.hpp>
#include <isl_utils.hpp>

#include <Codes/Exit_t.hpp>

If_t::If_t(const Code_t* _c, ISL::Set _context, bool _negate) : 
	Block_t(_c),
	context(_context),
	neg(_negate)
{}
	
std::string If_t::to_cpp_string() const{
	std::ostringstream r;
	
	r << "if(" << (neg ? "!(" : "") << parameter_to_str(context) << (neg ? ")" : "") << ")\n" << Block_t::to_cpp_string();
	return r.str();
}
