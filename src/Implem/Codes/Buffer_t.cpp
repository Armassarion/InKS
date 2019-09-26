#include <sstream>

#include <Codes/Buffer_t.hpp>
#include <isl_utils.hpp>
#include <utils.hpp>
#include <vector>

Buffer_t::Buffer_t(const std::string& _n, const std::string& _t, ISL::Set _s, int _do) :
	Buffer_t(_n, _t, _s, -1, _do)
{}

Buffer_t::Buffer_t(const std::string& _n, const std::string& _t, ISL::Set _s, int _di, int _do) :
	name(_n),
	type(_t),
	sizes_isl(ISL::copy(_s)),
	dim_in(_di),
	dim_out(_do)
{}

std::string Buffer_t::to_cpp_string() const{
	std::vector<std::vector<C_from_isl_t>> sizes_str(std::move(set_component_to_string(sizes_isl)));
	uint nb = sizes_str.size();
	std::ostringstream r;
	
	//Allocation
	r << name << " = new " << type << "[";
	for(uint i=0; i<nb; ++i){
		if(sizes_str[i].size() > 1){
			std::cout << "** Error: multiple possible sizes one of the " << name << " physical array : " << std::endl;
			for(const auto& parse_aff : sizes_str[i])
				std::cout << "\t" << parse_aff.condition << " -> " << parse_aff.val << std::endl;
			std::cout << "Set: " << isl_set_to_str(sizes_isl) << std::endl;
			exit(EXIT_FAILURE);
		}
		r << "((size_t)(" << sizes_str[i][0].val << "))" << add_str_if(i, nb, " * ");
	}
	r << "];\n";
	
	return r.str();
}
