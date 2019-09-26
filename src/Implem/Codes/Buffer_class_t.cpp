#include <sstream>

#include <Codes/Buffer_class_t.hpp>
#include <isl_utils.hpp>
#include <utils.hpp>

Buffer_class_t::Buffer_class_t(const std::string& _lid, ISL::UMap _memmap, const V_Input_t& _csts):
	lid(_lid),
	memmap(ISL::copy(_memmap)),
	csts(_csts)
{}

std::string Buffer_class_t::to_cpp_string() const{
	ISL::L_Map maps = isl_union_map_get_map_list(memmap);
	int nb_map = isl_map_list_size(maps);
	std::ostringstream r;
	
	r << lid << "_t " << lid << "(";
	
	for(int i=0; i<nb_map; i++){
		ISL::Map m = isl_map_list_get_map(maps, i);
		const char* pid = isl_map_get_tuple_name(m, isl_dim_out);
		if(pid == NULL){
			std::cerr << "***Warning: some ptr couldnt be done for the following mapping:" << std::endl;
			std::cerr << isl_union_map_to_str(memmap) << std::endl << std::endl;
			exit(EXIT_FAILURE);
		}
		
		r << pid;
		for(int dim=1; dim<isl_map_dim(m, isl_dim_out); dim++)
			r << ", " << pid << "_mult_" << dim;
	
		r << add_coma_if(i, nb_map);
	}
	
	
	if(csts.size() > 0){
		r << ", ";
		for(size_t i=0; i<csts.size(); ++i)
			r << csts[i].name << add_coma_if(i, csts.size());
	}
	r << ");\n";
	
	return r.str();
}