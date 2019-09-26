#include <sstream>

#include <V_Declaration_t.hpp>

void V_Declaration_t::push(Declaration_t* d){
	vdecl.push_back(d);
}

void V_Declaration_t::push(const std::string& _s){
	vdecl.emplace_back(new Declaration_t(_s));
}
	
std::string V_Declaration_t::to_cpp_decl() const{
	std::ostringstream r;
	for(const auto& d : vdecl)
		r << d->to_cpp_decl() << std::endl;
	return r.str();
}

void V_Declaration_t::merge(V_Declaration_t& _vdecl){
	vdecl.insert(vdecl.end(), _vdecl.begin(), _vdecl.end());
}


std::vector<Declaration_t*>::iterator V_Declaration_t::begin(){
	return vdecl.begin();
}

std::vector<Declaration_t*>::iterator V_Declaration_t::end(){
	return vdecl.end();
}

const std::vector<Declaration_t*>::const_iterator V_Declaration_t::begin() const{
	return vdecl.begin();
}

const std::vector<Declaration_t*>::const_iterator V_Declaration_t::end() const{
	return vdecl.end();
}