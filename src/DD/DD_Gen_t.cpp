#include <iostream>
#include <string>

#include <isl_cpp_type.hpp>
#include <isl_utils.hpp>
#include <DD_Gen_t.hpp>

DD_Gen_t::DD_Gen_t() :
	DD_Gen_t(empty_isl_union_set(), empty_isl_union_set())
{}

DD_Gen_t::DD_Gen_t(ISL::USet _data_gen, ISL::USet _data_read) :
	data_gen(ISL::copy(_data_gen)),
	data_read(ISL::copy(_data_read))
{}

DD_Gen_t& DD_Gen_t::operator+=(const DD_Gen_t& src){
	data_gen = isl_union_set_union(data_gen, src.data_gen);
	data_read = isl_union_set_union(data_read, src.data_read);
	return *this;
}

void DD_Gen_t::print() const{
	std::cout << "Gen_t" << std::endl;
	ISL::print(data_gen, "W: ");
	ISL::print(data_read, "R: ");
}

