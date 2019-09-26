#include <Codes/Delete_buffer_t.hpp>

Delete_buffer_t::Delete_buffer_t(const std::string& _id) : 
	id(_id)
{}
	
std::string Delete_buffer_t::to_cpp_string() const{
	return std::string("delete[] ") + id + ";\n";
}