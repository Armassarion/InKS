#include <Codes/Mult_array_t.hpp>
#include <isl_utils.hpp>
#include <utils.hpp>
#include <vector>

Mult_array_t::Mult_array_t(const std::string& _n, ISL::Set _s) : 
	name(_n),
	sizes_isl(ISL::copy(_s))
{}

std::string Mult_array_t::to_cpp_string() const{
	int nb = isl_set_dim(sizes_isl, isl_dim_set);
	std::ostringstream r;
	std::ostringstream var_base;
	if(nb > 1){
		std::vector<std::vector<C_from_isl_t>> sizes_str(std::move(set_component_to_string(sizes_isl)));
		std::vector<std::string> vec_mult(nb-1, sizes_str[nb-1][0].val);	
		
		for(int i=1; i<nb-1; i++)
			vec_mult[i] = "(" + vec_mult[i-1] + ") * (" + sizes_str[nb-i-1][0].val + ")";
		
		var_base << "size_t " << name << "_mult_";
		for(int i=0; i<nb-1; ++i)
			r << var_base.str() << i+1 << " = " << vec_mult[nb-i-2] << ";\n";
	}
	
	return r.str();
}

