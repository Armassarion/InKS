#ifndef V_DDFORNAMEDVAR_HPP
#define V_DDFORNAMEDVAR_HPP

#include <isl_cpp_type.hpp>
#include <vector>
#include <string>
#include <map>

class Constraint_t;
class V_Input_t;
class Point_t;
class List_t;
class DD_Scope_t;
class DDForOptions_t;

struct DDForNamedVar{
	
	DDForOptions_t* options;
	
	std::string id;
	
	Constraint_t* c;

	DDForNamedVar(DDForOptions_t* _opt, const char* _id, Constraint_t* _c);
	
};

class V_DDForNamedVar{
	
	std::vector<DDForNamedVar*> vars;
	
	Point_t* get_point() const;
	
public:
	
	static const char* DD_For_kernel_name;

	V_DDForNamedVar(DDForNamedVar* _var);
	
	V_DDForNamedVar* push(DDForNamedVar* _var);
		
	ISL::Map get_order_map(DD_Scope_t& scope_object) const;
	
	ISL::Map get_order_map_param(DD_Scope_t& scope_object) const;
	
	ISL::L_Id to_id_list() const;
	
	List_t* to_list() const;
	
	std::string add_str_before(const std::string& iterator_id);

	size_t size() const;

};

#endif // V_DDFORNAMEDVAR_HPP
