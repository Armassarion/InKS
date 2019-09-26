#include <iostream>
#include <sstream>
#include <tuple>

#include <Declarations/Buffer_class_decl_t.hpp>
#include <isl_utils.hpp>
#include <utils.hpp>

Buffer_class_decl_t::Buffer_class_decl_t(const std::string& _s, const std::string& _t, uint _id, const V_Input_t& _v, ISL::UMap _m) :
	logical_name(_s),
	type(_t),
	id(_id),
	Vin(_v),
	mapping(isl_union_map_coalesce(ISL::copy(_m)))
{}

Buffer_class_decl_t::Buffer_class_decl_t(const std::string& _s, const std::string& _t, const V_Input_t& _v, ISL::UMap _m) :
	Buffer_class_decl_t(_s, _t, 0, _v, _m)
{}

std::string Buffer_class_decl_t::to_cpp_decl() const{
	std::ostringstream r;
	r << "struct " << logical_name << "_t{\n";
	r << this->const_attr() << std::endl;
	r << this->ptr_attr() << std::endl;
	r << this->constructor() << std::endl;
	r << this->parenthesis_operator() << std::endl;
	r << "};\n";
	return r.str();
}

std::string Buffer_class_decl_t::const_attr() const{
	std::ostringstream r;
	for(const Input_t& I : Vin)
		r << "\tconst int & " << I.name << ";\n";
	return r.str();
}

std::string Buffer_class_decl_t::ptr_attr() const {
	ISL::L_Map lmap = isl_union_map_get_map_list(mapping);
	int nb = isl_map_list_size(lmap);
	std::ostringstream r;
	
	for(int i=0; i<nb; i++){
		ISL::Map m = isl_map_list_get_map(lmap, i);
		const char* physical_name = isl_map_get_tuple_name(m, isl_dim_out);
		if(physical_name == NULL){
			std::cerr << "***Error: some ptr attr couldnt be found for the following mapping :\n" << ISL::str(mapping) << std::endl;
			exit(EXIT_FAILURE);
		}
		
		int nb_dim = isl_map_dim(m, isl_dim_out);
		r << type;
		if(nb_dim > 0) 	r << "* const";
		r << "& " << physical_name << ";\n";
		
		for(int dim=1; dim<nb_dim; dim++)
			r << "\tconst size_t &" << physical_name << "_mult_" << dim << ";\n";
	}
	
	return r.str();
}

std::string Buffer_class_decl_t::parenthesis_operator() const {
	std::ostringstream r;
	r << "\t" << type << "& operator()(";
	//ISL::print(mapping);
	int nb_dim = isl_set_dim(isl_set_from_union_set(isl_union_map_domain(ISL::copy(mapping))), isl_dim_set);
	for(int i=0; i<nb_dim; ++i)
		r << "const int & " << "in_" << i << add_coma_if(i, nb_dim);
	r << ") const{\n\n";
	r << union_map_to_C(ISL::copy(mapping));
	r << "\n\t}\n";
	
	return r.str();
}

std::string Buffer_class_decl_t::constructor() const {
	std::ostringstream r;
	ISL::L_Map maps = isl_union_map_get_map_list(mapping);
	
	r << "\t" << logical_name << "_t(";
		r << this->ptr_param(maps);
		if(Vin.size() > 0){
			r << ", ";
			r << this->const_param();
		}
	r << ") : \n";
		r << this->ptr_setter(maps);
		if(Vin.size() > 0){
			r << ",\n";
			r << this->const_setter();
		}
	r << "\n{}\n";
	return r.str();
}

std::string Buffer_class_decl_t::ptr_param(ISL::L_Map maps) const {
	int nb_maps = isl_map_list_size(maps);
	std::ostringstream r;

	for(int i=0; i<nb_maps; i++){
		ISL::Map m = isl_map_list_get_map(maps, i);
		
		const char* physical_name = isl_map_get_tuple_name(m, isl_dim_out);
		if(physical_name == NULL){
			std::cerr << "***Warning: some ptr setter couldnt be done for the following mapping:" << std::endl;
			std::cerr << isl_union_map_to_str(mapping) << std::endl << std::endl;
			exit(EXIT_FAILURE);
		}
		
		int nb_dim = isl_map_dim(m, isl_dim_out);
		r << type;	
		if(nb_dim > 0) 	r << "* const";
		r << "& " << physical_name << "_param";
		
		for(int dim=1; dim<nb_dim; dim++)
			r << ", const size_t & " << physical_name << "_mult_" << dim << "_param";
		
		r << add_coma_if(i, nb_maps);
	}
	
	return r.str();
}

std::string Buffer_class_decl_t::const_param() const {
	std::ostringstream r;
	for(size_t i=0; i<Vin.size(); ++i)
		r << "const int & " << Vin[i].name << "_param" << add_coma_if(i, Vin.size());
	return r.str();
}

std::string Buffer_class_decl_t::ptr_setter(ISL::L_Map maps) const {
	int nb_maps = isl_map_list_size(maps);
	std::ostringstream r;

	for(int i=0; i<nb_maps; i++){
		ISL::Map m = isl_map_list_get_map(maps, i);
		const char* physical_name = isl_map_get_tuple_name(m, isl_dim_out);
		
		r << physical_name << "(" << physical_name << "_param)";
		if(isl_map_dim(m, isl_dim_out) > 1)
		for(int dim=1; dim<isl_map_dim(m, isl_dim_out); dim++)
			r << ",\n" << physical_name << "_mult_" << dim << "(" << physical_name << "_mult_" << dim << "_param)";
		
		r << add_str_if(i, nb_maps, ",\n");
	}
	
	return r.str();
}

std::string Buffer_class_decl_t::const_setter() const{
	std::ostringstream r;
	for(size_t i=0; i<Vin.size(); ++i)
		r << Vin[i].name << "(" << Vin[i].name << "_param)" << add_str_if(i, Vin.size(), ",\n");
	return r.str();
}