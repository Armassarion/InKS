#ifndef AUTO_BUFFER_DECL_T_HPP
#define AUTO_BUFFER_DECL_T_HPP

#include <string>
#include <vector>

#include <Declarations/Declaration_t.hpp>
#include <isl_cpp_type.hpp>
#include <Input_t.hpp>
#include <Array.hpp>

struct Auto_buffer_decl_t : public Declaration_t{
	
	std::string name;
	
	std::string type;
	
	int dim_in;
	
	int dim_out;	
	
	ISL::Set sizes_isl;
	
	V_Input_t Vin;
	
	std::vector<int> dim_index;
	
	isl_multi_pw_aff* isl_allocate_size;
	
//METHODS

	Auto_buffer_decl_t(const Array& Logical, const V_Input_t& _v, ISL::Set deltas, ISL::Set valid_parameters);

	std::string to_cpp_decl() const;

private:
	std::vector<bool> compute_use_modulo() const;

	std::string get_tuple_type() const;
	
	std::string get_bool_set() const;

	std::string get_mult_sizes(const std::vector<std::string> sizes) const;

	std::string get_dim_sizes(const std::vector<std::string> sizes) const;
	 
};

#endif // BUFFER_DECL_T_HPP
