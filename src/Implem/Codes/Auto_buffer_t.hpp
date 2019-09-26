#ifndef AUTO_BUFFER_HPP
#define AUTO_BUFFER_HPP

#include <string>
#include <vector>

#include <isl_cpp_type.hpp>

#include <Codes/Buffer_t.hpp>
#include <Input_t.hpp>

class Array;

class Auto_buffer_t : public Buffer_t{

	V_Input_t Vin;

public:
	
	Auto_buffer_t(const Array& _logical_ref, const V_Input_t& _v);
	
	std::string to_cpp_string() const;

};

#endif // AUTO_BUFFER_HPP
