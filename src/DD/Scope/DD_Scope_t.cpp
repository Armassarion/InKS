#include <iostream>

#include <Scope_exception_t.hpp>
#include <const_globals.hpp>
#include <DD_Scope_t.hpp>
#include <isl_utils.hpp>
#include <DD_Decl_t.hpp>
#include <Array_Map.hpp>
#include <DD_Mem_t.hpp>
#include <Var_t.hpp>

using std::cout;
using std::endl;

DD_Scope_t::DD_Scope_t() : 
	valid_parameters(nullptr)
{}

DD_Scope_t::DD_Scope_t(const DD_Scope_t& src){
	valid_parameters = ISL::copy(src.valid_parameters);
	Arrays = src.Arrays;
	vin = src.vin;
	Allocs_map_copy(src.allocs, src.allocs_keep);
	Decls_map_copy(src.decls);
	l_to_p = src.l_to_p;
	variables = src.variables;
}

DD_Scope_t& DD_Scope_t::operator=(const DD_Scope_t& src){
	valid_parameters = ISL::copy(src.valid_parameters);
	Arrays = src.Arrays;
	vin = src.vin;
	Allocs_map_copy(src.allocs, src.allocs_keep);
	Decls_map_copy(src.decls);
	l_to_p = src.l_to_p;
	variables = src.variables;
		
	return *this;
}

void DD_Scope_t::init(ISL::Set _vparameters, Array_Map* _Arrays, V_Input_t& _vin, ISL::USet _uTD){
	valid_parameters = ISL::copy(_vparameters);
	Arrays = _Arrays;
	vin = _vin;
}

void DD_Scope_t::Allocs_map_copy(const Allocs_map_t& src, const Allocs_map_t& src_keep){
	for(auto& pair : src)
		allocs[pair.first] = new DD_Mem_t(*pair.second);
	for(auto& pair : src_keep)
		allocs_keep[pair.first] = new DD_Mem_t(*pair.second);
}

void DD_Scope_t::Decls_map_copy(const Decls_map_t& src){
	for(auto& pair : src)
		decls[pair.first] = new DD_Decl_t(*pair.second);
}

const Array_Map* DD_Scope_t::array() const{
	return Arrays;
}

const Array& DD_Scope_t::array(const std::string& lid) const{
	return Arrays->at(lid);
}

bool DD_Scope_t::array_contains(const std::string& lid) const{
	return Arrays->contains(lid);
}

bool DD_Scope_t::array_is_dims(const std::string& lid, size_t dim) const{
	return array_contains(lid) && Arrays->at(lid).dim_size == dim;
}

void DD_Scope_t::array_set_is_consistent(ISL::USet uset){
	isl_union_set_foreach_set(uset, 
		[](ISL::Set set, void* user) -> isl_stat{
			DD_Scope_t* scope = (DD_Scope_t*)user;
			const char* cname = isl_set_get_tuple_name(set);
			if(cname == NULL) throw Scope_exception_t("Unnamed sets are forbidden!\n");
			size_t nb_dim = isl_set_dim(set, isl_dim_set);
			
			if(!scope->array_contains(cname))
				throw Scope_exception_t(std::string("Unknown logical array : ") + cname + "\n");
			if(!scope->array_is_dims(cname, nb_dim))
				throw Scope_exception_t(std::string("Number of dimensions mismatch\n"));
			
			return isl_stat_ok;
		}
	, this);
}
	
void DD_Scope_t::add_const(List_t* l){
	vin.merge(l);
}

void DD_Scope_t::add_mem(const DD_Mem_t& ddMem){
	allocs[ddMem.get_physical_id()] = new DD_Mem_t(ddMem);
}

void DD_Scope_t::add_mem_keep(const DD_Mem_t& ddMem){
	allocs_keep[ddMem.get_physical_id()] = new DD_Mem_t(ddMem);
}

void DD_Scope_t::sub_mem(const std::string& pid){
	allocs.erase(pid);
}

void DD_Scope_t::add_decl(const DD_Decl_t& ddDecl){
	decls[ddDecl.get_physical_id()] = new DD_Decl_t(ddDecl);
}

void DD_Scope_t::set_l_to_p(const std::string& lid, const std::string& pid){
	l_to_p[lid] = pid;
}
	
std::string DD_Scope_t::get_l_to_p(const std::string& lid) const{
	return l_to_p.at(lid);
}

size_t DD_Scope_t::var_size() const{
	return variables.size();
}

void DD_Scope_t::add_var(Var_t* v){
	variables.push(v);
}

List_var_t& DD_Scope_t::get_list_vars(){
	return variables;
}

Set_Poly_t* DD_Scope_t::get_poly(Set_Poly_t* p){
	return variables.get_poly(p);
}

std::string DD_Scope_t::get_var_name(int i) const{
	return variables.get_var_name(i);
}

List_t DD_Scope_t::get_var_names() const{
	return variables.get_var_names();
}

List_t DD_Scope_t::get_all_vars() const{
	List_t r = variables.get_var_names();
	for(const auto& i : vin)
		r.push(i.name);
	return r;
}

const std::map<std::string, std::string>& DD_Scope_t::get_l_to_p() const{
	return l_to_p;
}
	
const V_Input_t& DD_Scope_t::get_const() const{
	return vin;
}

bool DD_Scope_t::logical_contains(const std::string& lid) const{
	return Arrays->contains(lid);
}

bool DD_Scope_t::alloc_contains(const std::string& id) const {
	return allocs.count(id) || allocs_keep.count(id);
}

const DD_Mem_t* DD_Scope_t::get_alloc(const std::string& pid) const{
	if(allocs.count(pid)){
		return allocs.at(pid);
	}else if(allocs_keep.count(pid)){
		return allocs_keep.at(pid);
	}
	throw Scope_exception_t(std::string("Search in scope error: ") + pid + " physical arrays doesn't exists.");
}

bool DD_Scope_t::alloc_is_not_const(const std::string& id) const {
	return allocs.count(id);
}

bool DD_Scope_t::decl_contains(const std::string& id) const {
	return decls.count(id);
}

bool DD_Scope_t::decl_contains(const std::string& id, const std::string& type, int dim) const {
	return decls.count(id) && decls.at(id)->match(type, dim);
}

int DD_Scope_t::decl_dim_size(const std::string& id) const{
	return decls.at(id)->get_dim();
}

void DD_Scope_t::intersect_parameters(ISL::USet& us) const{
	us = isl_union_set_intersect_params(us, ISL::copy(valid_parameters));
}

#define PRINT_ALL			 0
#define PRINT_CSTS			(0 || PRINT_ALL)
#define PRINT_DECL			(0 || PRINT_ALL)
#define PRINT_ALLOCS		(0 || PRINT_ALL)
#define PRINT_MODIFICATIONS	(0 || PRINT_ALL)
#define PRINTABLE_SCOPE	(PRINT_MODIFICATIONS || PRINT_ALLOCS || PRINT_DECL || PRINT_CSTS)

void DD_Scope_t::print(const std::string& prefix, int lineno) const{
	if(PRINTABLE_SCOPE){
		if(lineno != -1) cout << prefix << " (line " << lineno << ")\n";
		else			 cout << prefix << "\n";
		
		if(PRINT_CSTS){
			cout << "Vin: \n";
			vin.print();
		}
				
		if(PRINT_ALLOCS){
			cout << "Allocs: \n";
			for(const auto& am : allocs){
				cout << am.first << ": ";
				am.second->print();
			}	
			
			cout << "Allocs keep: \n";
			for(const auto& am : allocs_keep){
				cout << am.first << ": ";
				am.second->print();
			}
		}
		
		if(PRINT_DECL){
			cout << "Decls: \n";
			for(const auto& d : decls)
				d.second->print();
		}
		
		if(PRINT_MODIFICATIONS){
			cout << "Modifications\n";
			//modifications.print();
		}

		cout << endl;
	}
}