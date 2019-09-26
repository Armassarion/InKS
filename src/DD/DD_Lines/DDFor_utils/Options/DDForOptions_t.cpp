#include <sstream>

#include <DDFor_utils/Options/DDForOptions_t.hpp>
#include <DDFor_utils/Options/DDForOption_t.hpp>

DDForOptions_t::DDForOptions_t(DDForOption_t* _opt){
	options.push_back(_opt);
}
	
DDForOptions_t* DDForOptions_t::push(DDForOption_t* _opt){
	options.push_back(_opt);
	return this;
}

std::string DDForOptions_t::add_str_before(){
	std::ostringstream r;
	for(auto& opt : options)
		r << opt->add_str_before();
	return r.str();
}