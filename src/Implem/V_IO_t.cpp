#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include <const_globals.hpp>
#include <V_IO_t.hpp>
#include <utils.hpp>

io_auto_str V_IO_t::to_auto_string(){
	std::ostringstream tmpl, param;
	
	for(uint i=0; i<size(); i++){
		tmpl << "typename T" << i << add_coma_if(i, size());
		param << vio[i].to_auto_string(i) << add_coma_if(i, size());
	}
	
	return io_auto_str{tmpl.str(), param.str()};
}

std::string V_IO_t::to_cpp_string(){
	std::ostringstream r;
	for(uint i=0; i<size(); i++)
		r << vio[i].to_cpp_string() << add_coma_if(i, size());
	return r.str();
}

void V_IO_t::push(IO_t&& io){
	vio.emplace_back(std::move(io));
}

size_t V_IO_t::size() const{
	return vio.size();
}
