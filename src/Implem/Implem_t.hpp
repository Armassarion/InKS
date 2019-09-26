#ifndef IMPLEM_T_HPP
#define IMPLEM_T_HPP

#include <string>

#include <V_Declaration_t.hpp>
#include <isl_cpp_type.hpp>
#include <V_Code_t.hpp>
#include <Input_t.hpp>
#include <V_IO_t.hpp>

class Implem_t{
	
public:

	std::string name;
	
	V_Input_t const_params;
	
	V_Declaration_t declarations;

	V_IO_t array_params;
	
	V_Code_t codes;
	
	Implem_t(Implem_t&&);
	
	Implem_t();
	
	Implem_t(std::string _name, const V_Input_t& vi);
	
	void push_io(IO_t&& io);
	
	void push_io(IO_t& io);

	void push_decl(const std::string& decl);

	void push_decl(Declaration_t* decl);
	
	void push_decl(V_Declaration_t& _vdecl);

	void push_code_front(Code_t* code);
	
	void push_code(Code_t* code);

	void push_code(const std::string& code);
	
	void push_condition_code(const std::string& code);

	std::string to_auto_string();
	
	std::string to_cpp_string();
	
	std::string get_code_cpp_string() const;
	
private:

	std::string get_auto_prototype();
	
	std::string get_cpp_prototype();

};

#endif // IMPLEM_T_HPP
