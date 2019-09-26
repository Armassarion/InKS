#include <utility>

#include <Codes/Auto_buffer_t.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>
#include <Array.hpp>
#include <utils.hpp>

using oss = std::ostringstream;
using std::string;

Auto_buffer_t::Auto_buffer_t(const Array& _logical_ref, const V_Input_t& _v) : 
	Buffer_t(_logical_ref.name, _logical_ref.type, ISL::copy(_logical_ref.isl_set_max_value), _logical_ref.dim_size),
	Vin(_v)
{}

std::string Auto_buffer_t::to_cpp_string() const{
	oss declaration;

	if(dim_out > 0)	declaration << "\t" << name << "_t " << name << "(" << Vin.get_params() << ");\n";
	else			declaration << "\t" << type << " " << name << ";\n";

	return declaration.str();
}
