#ifndef DD_SCOPE_T_HPP
#define DD_SCOPE_T_HPP

#include <string>
#include <map>

#include <V_Modification_t.hpp>
#include <isl_cpp_type.hpp>
#include <List_var_t.hpp>
#include <Input_t.hpp>
#include <List_t.hpp>

class Set_Poly_t;
class Array_Map;
class DD_Decl_t;
class DD_Mem_t;
class List_t;
class Array;
class Var_t;

using Allocs_map_t = std::map<std::string, DD_Mem_t*>;
using Decls_map_t = std::map<std::string, DD_Decl_t*>;

class DD_Scope_t{
	
	ISL::Set valid_parameters;

	Array_Map* Arrays;
	
	V_Input_t vin;
	
	Allocs_map_t allocs;
	
	Allocs_map_t allocs_keep;
	
	Decls_map_t decls;
	
	//list of existing variables 
	List_var_t variables;
	
	std::map<std::string, std::string> l_to_p;
	
public:
	
	V_Modification_t modifications;
	
	DD_Scope_t();

	DD_Scope_t(const DD_Scope_t& src);
	
	DD_Scope_t& operator=(const DD_Scope_t& src);
	
	void init(ISL::Set _vparameters, Array_Map* _Arrays, V_Input_t& _vin, ISL::USet _uTD);

	void Allocs_map_copy(const Allocs_map_t& src, const Allocs_map_t& src_keep);
	
	void Decls_map_copy(const Decls_map_t& src);
	
	const Array_Map* array() const;
	
	const Array& array(const std::string& lid) const;
	
	bool array_contains(const std::string& lid) const;
	
	bool array_is_dims(const std::string& lid, size_t dim) const;
	
	void array_set_is_consistent(ISL::USet uset);
	
	void add_const(List_t*);
	
	void add_mem(const DD_Mem_t& ddMem);
	
	void add_mem_keep(const DD_Mem_t& ddMem);
	
	void sub_mem(const std::string& pid);
	
	void add_decl(const DD_Decl_t& ddDecl);
	
	void set_l_to_p(const std::string&, const std::string&);
	
	std::string get_l_to_p(const std::string&) const;
	
	size_t var_size() const;
	
	void add_var(Var_t* v);
	
	List_var_t& get_list_vars();
	
	Set_Poly_t* get_poly(Set_Poly_t* p);
	
	std::string get_var_name(int i) const;
	
	List_t get_var_names() const;
	
	List_t get_all_vars() const;
	
	const std::map<std::string, std::string>& get_l_to_p() const;
		
	const V_Input_t& get_const() const;
	
	bool logical_contains(const std::string& lid) const;
	
	bool alloc_contains(const std::string& id) const;

	const DD_Mem_t* get_alloc(const std::string& pid) const;
	
	bool alloc_is_not_const(const std::string& id) const;

	bool decl_contains(const std::string& id) const;
	
	int decl_dim_size(const std::string& id) const;
	
	bool decl_contains(const std::string& id, const std::string& type, int dim) const;
	
	void intersect_parameters(ISL::USet& us) const;
	
	uint const_size() const{
		return vin.size();
	}
	
	void print(const std::string& prefix="", int lineno=-1) const;
};

#endif // DD_SCOPE_T_HPP
