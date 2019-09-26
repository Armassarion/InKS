#ifndef ISL_UTILS_HPP
#define ISL_UTILS_HPP

#include <assert.h>
#include <string>
#include <vector>

#include <isl_cpp_type.hpp>

struct C_from_isl_t{
	std::string condition;
	std::string val;
};

struct Map_to_C{
	std::vector<C_from_isl_t> c_str;
	bool skip_condition;
	
	Map_to_C(bool _s) : skip_condition(_s){}
	
	void push(C_from_isl_t c){
		c_str.push_back(c);
	}
};

ISL::Printer alloc_isl_printer();
std::string get_isl_macro();
std::string printer_get_str(ISL::Printer& p);
std::string print_C_ast_expr(ISL::Ast_expr expr);
	
isl_stat bset_to_C(ISL::BSet bset, void* user);
isl_stat bmap_to_C(ISL::BMap bmap, void* user);
isl_stat aff_to_C(ISL::Set set, ISL::Aff wf, void* user);

std::vector<C_from_isl_t> pw_aff_to_C(__isl_take ISL::PWAff pw_aff);
std::vector<std::vector<C_from_isl_t>> multi_pw_aff_to_C(__isl_take ISL::M_PWAff multi_pw_aff);

void vsize_from_isl_pw_aff(std::vector<std::string>& vsize, __isl_take ISL::PWAff pw_aff);
void vsize_from_isl_multi_pw_aff(std::vector<std::string>& vsize, __isl_take ISL::M_PWAff multi_pw_aff);

std::string get_map_dim_name(ISL::Map m, isl_dim_type t, int dim, std::string prefix="");
	
std::string parameter_to_str(ISL::Set, std::string="");
bool bounded_set(ISL::Set set, size_t& unbounded_dim);
bool dim_bounded_set(ISL::Set set, size_t unbounded_dim);

//Return a C++ function that map the input to a value in output array
isl_stat map_to_C(ISL::Map map, void* user);
std::string union_map_to_C(ISL::UMap umap);

std::vector<std::vector<C_from_isl_t>> set_component_to_string(__isl_keep ISL::Set);
void name_map_in_from_set(ISL::Map& map, ISL::Set set);

ISL::USet union_set_complement(ISL::USet);

ISL::LSpace lspace_from_map(ISL::Map);
ISL::LSpace lspace_from_set(ISL::Set);
ISL::Set empty_isl_set(int=0);
ISL::Map empty_isl_map(int=0, int=0);
ISL::USet empty_isl_union_set();
ISL::UMap empty_isl_union_map();
ISL::Set universe_isl_param_set();
ISL::Set universe_isl_set(int dim =0);
ISL::Map universe_isl_map(int=0, int=0);

ISL::Map drop_param_constraint(ISL::Map map);
ISL::Set drop_param_constraint(ISL::Set set);
ISL::UMap drop_param_constraint(ISL::UMap umap);
ISL::USet drop_param_constraint(ISL::USet uset);
bool union_map_has_single_bmap(ISL::UMap umap);
bool union_set_has_single_bset(ISL::USet uset);

void date_union_map_n_lex_lt(ISL::UMap& um, int size, int start1, int start2);
void date_union_map_n_lex_gt_0(ISL::UMap& um, int size, int start1);
void date_union_map_n_lex_le(ISL::UMap& um, int size, int start1, int start2);
void date_union_map_n_equality(ISL::UMap& um, int size, int start1, int start2);
void date_union_map_project(ISL::UMap& um, int size, int start);
void date_union_map_insert_dims(ISL::UMap& um, int size, int start=-1);

template <typename T>
void foreach_map(ISL::UMap um, T&& apply){
	ISL::L_Map lmaps = isl_union_map_get_map_list(um);
	int nb_map = isl_map_list_size(lmaps);
	for(int id_map=0; id_map<nb_map; id_map++)
		apply(isl_map_list_get_map(lmaps, id_map));
	ISL::free(lmaps);
}

template <typename T>
void foreach_set(ISL::USet us, T&& apply){
	ISL::L_Set lsets = isl_union_set_get_set_list(us);
	int nb_set = isl_set_list_size(lsets);
	for(int id_set=0; id_set<nb_set; id_set++)
		apply(isl_set_list_get_set(lsets, id_set));
	ISL::free(lsets);
}

template <typename T>
void foreach_bmap(ISL::Map um, T&& apply){
	ISL::L_BMap lmaps = isl_map_get_basic_map_list(um);
	int nb_map = isl_basic_map_list_size(lmaps);
	for(int id_map=0; id_map<nb_map; id_map++)
		apply(isl_basic_map_list_get_basic_map(lmaps, id_map));
	ISL::free(lmaps);
}

template <typename T>
void foreach_bset(ISL::Set us, T&& apply){
	ISL::L_BSet lsets = isl_set_get_basic_set_list(us);
	int nb_set = isl_basic_set_list_size(lsets);
	for(int id_set=0; id_set<nb_set; id_set++)
		apply(isl_basic_set_list_get_basic_set(lsets, id_set));
	ISL::free(lsets);
}

template <typename T>
void date_union_map_n_apply(ISL::UMap& um, T&& apply){
	ISL::L_Map lmaps = isl_union_map_get_map_list(um);
	int nb_map = isl_map_list_size(lmaps);
	ISL::free(um);
	um = empty_isl_union_map();
	for(int id_map=0; id_map<nb_map; id_map++){
		ISL::Map map = isl_map_list_get_map(lmaps, id_map);
		apply(map);
		um = isl_union_map_union(um, isl_union_map_from_map(map));
	}
	um = isl_union_map_coalesce(um);
	ISL::free(lmaps);
}

template <typename T>
void date_union_set_n_apply(ISL::USet& us, T&& apply){
	ISL::L_Set lsets = isl_union_set_get_set_list(us);
	int nb_set = isl_set_list_size(lsets);
	ISL::free(us);
	us = empty_isl_union_set();
	for(int id_set=0; id_set<nb_set; id_set++){
		ISL::Set s = isl_set_list_get_set(lsets, id_set);
		apply(s);
		us = isl_union_set_union(us, isl_union_set_from_set(s));
	}
	us = isl_union_set_coalesce(us);
	ISL::free(lsets);
}

void date_union_map_self_cartesian_produit(ISL::UMap& um, int sch_size=-1, int start=0);
void date_union_map_intersect_range_wrapped(ISL::UMap& um, ISL::UMap& um2);
void date_union_map_intersect_domain_wrapped(ISL::UMap& um, ISL::UMap& um2);

void date_map_n_lex_lt(ISL::Map& m, int size, int start1, int start2);
void date_map_n_lex_le(ISL::Map& m, int size, int start1, int start2);
void date_map_n_equality(ISL::Map& m, int size, int start1, int start2);

void add_dims_to_0(ISL::Set& s, int nb);
void add_dims_to_0(ISL::Map& m, isl_dim_type t, int nb);

void set_add_value_to_dim(ISL::Set& set, ISL::PWAff v, int dim);
void set_mul_value_to_dim(ISL::Set& set, int v, int dim);
void set_add_value_to_dim(ISL::Set& set, int v, int dim);
void set_set_dim_to_const(ISL::Set& set, int cst, int dim);
void map_dim_equal(ISL::Map& m, int in, int out, isl_dim_type tin=isl_dim_in, isl_dim_type tout=isl_dim_out);
ISL::Set set_identity_to_param(ISL::Space spc);
ISL::Map map_identity_to_param(ISL::Space spc, isl_dim_type type);

bool basic_set_has_inequality(ISL::BSet bset);

bool elem_is_zero(ISL::Mat, int row, int col);
bool elem_is_not_zero(ISL::Mat mat, int row, int col);
bool one_elem_is_not_zero(ISL::Mat mat, int row, int start, int end);
bool only_one_elem_is_not_zero(ISL::Mat mat, int row, int start, int end);
int first_elem_is_not_zero(ISL::Mat mat, int row, int start, int end);
bool has_constraint(ISL::Mat, int row, int col1, int col2);
bool vals_are_same_sign(ISL::Val v1, ISL::Val v2);

#endif
