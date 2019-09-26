#include <DDFor_utils/Options/OMP_TaskLoop_option_t.hpp>
#include <List_t.hpp>
#include <utils.hpp>
#include <sstream>

OMP_TaskLoop_option_t::OMP_TaskLoop_option_t(const char* _collapse, const char* _grainSize, List_t* _privates, bool _parallel) : 
	privates(_privates),
	collapse(_collapse),
	grainSize(_grainSize),
	parallel(_parallel)
{}

std::string OMP_TaskLoop_option_t::add_str_before(){
	std::ostringstream r;
	r << "#pragma omp";
	if(parallel) r << " parallel";
	r << " taskloop collapse(" << collapse << ") grainsize(" << grainSize << ")";
	if(privates != nullptr && privates->size() > 0){
		r << " private(";
		for(size_t i=0; i<privates->size(); ++i)
			r << privates->get(i) << add_coma_if(i, privates->size());
		r << ")";
	}
	r << "\n";
	return r.str();
}