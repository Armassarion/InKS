#include <sstream>

#include <Declarations/Auto_buffer_decl_t.hpp>
#include <isl_utils.hpp>
#include <utils.hpp>

extern "C" {
	#include "lattifold.h"
}

Auto_buffer_decl_t::Auto_buffer_decl_t(const Array& Logical, const V_Input_t& _v, ISL::Set deltas, ISL::Set valid_parameters) : 
	name(Logical.name),
	type(Logical.type),
	dim_in(Logical.dim_size),
	sizes_isl(ISL::copy(Logical.isl_set_max_value)),
	Vin(_v)
{
	if(dim_in > 0){
		ISL::Set ndeltas = isl_set_coalesce(isl_set_neg(ISL::copy(deltas)));
		deltas = isl_set_coalesce(isl_set_union(deltas, ndeltas));
		
		// Computing
		lattifold_mapping *mapping = lattifold_mapping_from_deltas(deltas);
		
		//mapping function (M)
		isl_multi_aff* mat_map = lattifold_mapping_get_projection(mapping);
		dim_out = isl_multi_aff_dim(mat_map, isl_dim_out);
		
		//Converter generation (matrix)
		dim_index.reserve(dim_out);
		for(int i = 0; i<dim_out; i++){
			ISL::Aff aff = isl_multi_aff_get_aff(mat_map, i);
			for(int j=0; j<dim_in; j++){
				if(isl_val_is_one(isl_aff_get_coefficient_val(aff, isl_dim_in, j)) == isl_bool_true) dim_index[i] = j;
			}
		}

		//modulo function
		isl_allocate_size = isl_multi_pw_aff_coalesce(isl_multi_pw_aff_gist_params(lattifold_mapping_get_moduli(mapping), ISL::copy(valid_parameters)));
		isl_allocate_size = isl_multi_pw_aff_intersect_params(isl_allocate_size, ISL::copy(valid_parameters));
		isl_allocate_size = isl_multi_pw_aff_coalesce(isl_multi_pw_aff_gist_params(isl_allocate_size, ISL::copy(valid_parameters)));
	}
}

std::string Auto_buffer_decl_t::to_cpp_decl() const{
	std::ostringstream r;
	if(dim_in > 0){	
		std::vector<std::string> allocate_sizes;
		std::ostringstream InKSArr_spec;
		
		vsize_from_isl_multi_pw_aff(allocate_sizes, ISL::copy(isl_allocate_size));
		
		InKSArr_spec << "InKSArray<" << type << ", " << get_tuple_type() << ", " << get_bool_set() << ">";
		r << "\nstruct " << name << "_t : " << InKSArr_spec.str() << "{\n\t";
		r << name << "_t(" << Vin.get_params(true);
		r << "):" << InKSArr_spec.str() << "(\n\t";
		r << "{" << get_mult_sizes(allocate_sizes) << "},\n\t";
		r << get_dim_sizes(allocate_sizes) << "){}\n};" << std::endl;
	}
	return r.str();
}

std::vector<bool> Auto_buffer_decl_t::compute_use_modulo() const{
	std::vector<bool> use_modulo(dim_out);
	for(int i=0; i<dim_out; i++){
		ISL::PWAff physical_max_value = isl_pw_aff_coalesce(isl_multi_pw_aff_get_pw_aff(isl_allocate_size, i));
		ISL::PWAff logical_max_value = isl_pw_aff_coalesce(isl_set_dim_max(ISL::copy(sizes_isl), dim_index[i]));
		ISL::Set s = isl_pw_aff_gt_set(logical_max_value, physical_max_value);
		/*if is empty => remove modulo which is translated into "false" => don't use modulo*/
		use_modulo[i] = (isl_set_is_empty(s) != isl_bool_true);
	}
	return use_modulo;
}

std::string Auto_buffer_decl_t::get_tuple_type() const{
	std::string tuple_type("::std::tuple<");
	for(int i = 0; i<dim_in; i++){
		tuple_type += "LogToPhy<";
		for(int j=0; j<dim_in; j++)
			tuple_type += ((dim_index[i] == j) ? "1" : "0") + add_coma_if(j, dim_in);
		tuple_type += ">" + add_coma_if(i, dim_in);
	}
	return tuple_type + ">";
}
	
std::string Auto_buffer_decl_t::get_bool_set() const{
	std::vector<bool> use_modulo = compute_use_modulo();
	std::ostringstream bool_set;
	for(int i = 0; i<dim_in; i++)
		bool_set << (use_modulo[i] ? "true" : "false") << add_coma_if(i, dim_in);
	return bool_set.str();
}

std::string Auto_buffer_decl_t::get_mult_sizes(const std::vector<std::string> sizes) const{
	std::ostringstream r, multiplier;
	r << "1" << add_coma_if(0, dim_out);
	for(int i=1; i<dim_out; i++){
		multiplier << "(" << sizes[i-1] << ")";
		r << multiplier.str() << add_coma_if(i, dim_out);
		multiplier << " * ";
	}
	return r.str();
}

std::string Auto_buffer_decl_t::get_dim_sizes(const std::vector<std::string> sizes) const{
	std::ostringstream r;
	for(size_t i=0; i<sizes.size(); i++)
		r << "\t(uint) " << sizes[i] << add_str_if(i, sizes.size(), ",\n");
	return r.str();
}

