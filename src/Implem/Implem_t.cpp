#include <sstream>

#include <isl_utils.hpp>
#include <Implem_t.hpp>
#include <utils.hpp>

using oss = std::ostringstream;
using std::string;
using std::endl;	
	
Implem_t::Implem_t(Implem_t&& im) : 
	name(std::move(im.name)),
	const_params(std::move(im.const_params)),
	declarations(std::move(im.declarations)),
	array_params(std::move(im.array_params)),
	codes(std::move(im.codes))
{}

Implem_t::Implem_t(std::string _name, const V_Input_t& vi) : 
	name(_name),
	const_params(vi)
{}

Implem_t::Implem_t()
{}

void Implem_t::push_io(IO_t& io){
	array_params.push(std::move(io));
}

void Implem_t::push_io(IO_t&& io){
	array_params.push(std::move(io));
}

void Implem_t::push_decl(const std::string& decl){
	declarations.push(decl);
}

void Implem_t::push_decl(V_Declaration_t& _vdecl){
	declarations.merge(_vdecl);
}

void Implem_t::push_decl(Declaration_t* decl){
	declarations.push(decl);
}

void Implem_t::push_code_front(Code_t* code){
	codes.push_front(code);
}

void Implem_t::push_code(Code_t* code){
	codes.push(code);
}

void Implem_t::push_code(const std::string& code){
	codes.push(code);
}

std::string Implem_t::get_auto_prototype(){
	io_auto_str array_param_str = array_params.to_auto_string();
	oss prototype;
	
	prototype << "template <" << array_param_str.tmpl << ">\n";
	prototype << "void " << name << "(" << array_param_str.param;
	if(array_params.size() > 0 && const_params.size() > 0) prototype << ", ";
	prototype << const_params.get_params(true) << ")\n";
	
	return prototype.str();
}

std::string Implem_t::get_cpp_prototype(){
	oss prototype;
	
	prototype << "void " << name << "(" << array_params.to_cpp_string();
	if(array_params.size() > 0 && const_params.size() > 0) prototype << ", ";
	prototype << const_params.get_params(true) << ")\n";
	
	return prototype.str();
}

std::string Implem_t::to_auto_string(){
	oss r;
	
	r << declarations.to_cpp_decl();
	r << get_auto_prototype() << "{\n";
	r << codes.to_cpp_string();
	r << "\n\n}";
	
	return r.str();
}

std::string Implem_t::to_cpp_string(){
	oss r;
	
	r << declarations.to_cpp_decl();
	r << get_cpp_prototype() << "{\n";
	r << codes.to_cpp_string();
	r << "\n\n}";
	
	return r.str();
}

std::string Implem_t::get_code_cpp_string() const{
	return codes.to_cpp_string();
}