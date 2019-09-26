#include <Codes/OMP_Parallel_Region_t.hpp>
#include <List_t.hpp>
#include <utils.hpp>
#include <sstream>

OMP_Parallel_Region_t::OMP_Parallel_Region_t(List_t* _privates, const std::string& _inner) : 
	privates(_privates),
	inner_code(_inner)
{}

std::string OMP_Parallel_Region_t::to_cpp_string() const{
	std::ostringstream r;
	r << "#pragma omp parallel";
	if(privates != nullptr && privates->size() > 0){
		r << " private(";
		for(size_t i=0; i<privates->size(); ++i)
			r << privates->get(i) << add_coma_if(i, privates->size());
		r << ")";
	}
	r << "\n{\n\n" << inner_code << "\n\n}\n\n";
	return r.str();
}