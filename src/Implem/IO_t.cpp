#include <sstream>

#include <IO_t.hpp>

IO_t::IO_t(std::string _n, std::string _t, uint _d, bool _is_const) :
	name(_n),
	type(_t),
	dims(_d),
	is_const(_is_const)
{}

IO_t::IO_t(IO_t&& io) :
	name(std::move(io.name)),
	type(std::move(io.type)),
	dims(std::move(io.dims)),
	is_const(std::move(io.is_const))
{}

std::string IO_t::to_auto_string(uint i){
	std::ostringstream r;
	r << "T" << i << (dims > 0 ? "& " : " ") << "InOut_" << name;
	return r.str();
}

std::string IO_t::to_cpp_string(){
	std::ostringstream r;
	r << type;
	if(dims > 0) r << "*";
	r << " ";
	if(is_const == true) r << "const ";
	r << "& " << name;
	return r.str();
}

IO_t& IO_t::operator=(const IO_t& b){
	name = b.name;
	type = b.type;
	dims = b.dims;
	
	return *this;
}