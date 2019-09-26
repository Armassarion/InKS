#include <DDFor_utils/Options/OMP_For_option_t.hpp>
#include <List_t.hpp>
#include <utils.hpp>
#include <sstream>

OMP_For_option_t::OMP_For_option_t(const char* _collapse, List_t* _privates, bool _parallel) : 
	privates(_privates),
	collapse(_collapse),
	parallel(_parallel)
{}

std::string OMP_For_option_t::add_str_before(){
	std::ostringstream r;
	r << "#pragma omp";
	if(parallel) r << " parallel";
	r << " for schedule(static) collapse(" << collapse << ")";
	if(privates != nullptr && privates->size() > 0){
		r << " private(";
		for(size_t i=0; i<privates->size(); ++i)
			r << privates->get(i) << add_coma_if(i, privates->size());
		r << ")";
	}
	r << "\n";
	return r.str();
}