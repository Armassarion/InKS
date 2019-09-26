#include <iostream>
#include <sstream>
#include <numeric>
#include <cmath>

#include <isl/ctx.h>

#include <const_globals.hpp>
#include <isl_utils.hpp>
#include <List_t.hpp>
#include <utils.hpp>

using VC_from_isl = std::vector<C_from_isl_t>;
using oss = std::ostringstream;
using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

ISL::Printer alloc_isl_printer(){
	ISL::Printer p = isl_printer_to_str(ctx);
	p = isl_printer_set_output_format(p, ISL_FORMAT_C);
	
	p = isl_ast_op_type_set_print_name(p, isl_ast_op_min, "isl_min");
	p = isl_ast_op_type_set_print_name(p, isl_ast_op_max, "isl_max");
	p = isl_ast_op_type_set_print_name(p, isl_ast_op_fdiv_q, "isl_floord");
	
	return p;
}

string get_isl_macro(){	
	ISL::Printer p = alloc_isl_printer();
	
	p = isl_ast_op_type_print_macro(isl_ast_op_max, p);
	p = isl_ast_op_type_print_macro(isl_ast_op_min, p);
	p = isl_ast_op_type_print_macro(isl_ast_op_fdiv_q, p);
	
	std::string Modulo_operation =	"inline int InKS_Modulo(int a, int b){int r = a % b; return r < 0 ? r + b : r;}\n";
	
	return std::string(isl_printer_get_str(p)) + Modulo_operation;
}

std::string printer_get_str(ISL::Printer& p){
	std::string r(isl_printer_get_str(p));
	p = isl_printer_flush(p);
	return r;
}

std::string print_C_ast_expr(ISL::Ast_expr expr){
	ISL::Printer p = alloc_isl_printer();
	p = isl_printer_print_ast_expr(p, expr);
	return printer_get_str(p);
}

isl_stat bset_to_C(ISL::BSet bset, void* user){
	if(isl_basic_set_dim(bset, isl_dim_div) > 0){
		cerr << "*Warning: A set is partially skipped because of the presence of division." << endl;
		ISL::print(bset);
		bset = isl_basic_set_remove_divs(bset);
	}
	
	std::vector<ISL::Mat(*)(ISL::BSet, isl_dim_type, isl_dim_type, isl_dim_type, isl_dim_type)> mat_func;
	std::vector<string> operations({" == ", " >= "});
	List_t dim_name(bset, List_t::Add_param::T);
	std::string* condition = (std::string*)user;
	std::string basic_condition;
	
	mat_func.push_back(isl_basic_set_equalities_matrix);
	mat_func.push_back(isl_basic_set_inequalities_matrix);
	
	for(int op=0; op<2; ++op){
		ISL::Mat mat = mat_func[op](bset, isl_dim_cst, isl_dim_param, isl_dim_set, isl_dim_div);
		for(int i=0; i<isl_mat_rows(mat); ++i){
			string right_term, left_term;
			for(int j=0; j<isl_mat_cols(mat); ++j){
				int val = isl_val_get_num_si(isl_mat_get_element_val(mat, i, j));
				
				if(val != 0){
					oss to_add;
					int coef = abs(val);
					if(j!=0){
						if(coef != 1) to_add << coef << " * ";
						to_add << dim_name.get(j-1);
					}else{
						to_add << coef;
					}
					
					if(val < 0) right_term += to_add.str() + " + ";
					else 		left_term +=  to_add.str() + " + ";
				}
			}
			if(right_term.empty())	right_term = "0";
			else					delete_last(right_term, 3);
			if(left_term.empty())	left_term = "0";
			else 					delete_last(left_term, 3);
			
			basic_condition += " && (" + left_term + operations[op] + right_term + ")";
		}
	}
	
	basic_condition.erase(0, 4);
	*condition = *condition + " || (" + basic_condition + ")";
	return isl_stat_ok;
}

isl_stat bmap_to_C(ISL::BMap bmap, void* user){
	std::vector<std::string> computations(isl_basic_map_dim(bmap, isl_dim_out));
	Map_to_C* param = (Map_to_C*)user;
	List_t dim_name(bmap, List_t::Add_param::T);
	std::string basic_computation;
	std::string basic_condition;
	std::ostringstream p;
	ISL::Mat mat;
	const char* name = isl_basic_map_get_tuple_name(bmap, isl_dim_out);
	std::string Array_name(name != NULL ? name : "");
		
	int start_param = 1;
	int end_param = isl_basic_map_dim(bmap, isl_dim_param) + isl_basic_map_dim(bmap, isl_dim_in) + start_param;
	int start_out = end_param;
	int end_out =  isl_basic_map_dim(bmap, isl_dim_out) + start_out;
	
	mat = isl_basic_map_equalities_matrix(bmap, isl_dim_cst, isl_dim_param, isl_dim_in, isl_dim_out, isl_dim_div);
	int rows = isl_mat_rows(mat);
	int cols = isl_mat_cols(mat);
	
	for(int i=0; i<rows; ++i){
		oss right_term;

		//if it is an index computation
		if(one_elem_is_not_zero(mat, i, start_out, end_out)){
			int index = first_elem_is_not_zero(mat, i, start_out, end_out);
			ISL::Val dim_coef = isl_mat_get_element_val(mat, i, index);
			
			for(int j=0; j<end_param; ++j){
				ISL::Val v = isl_mat_get_element_val(mat, i, j);
				
				if(isl_val_is_zero(v) == isl_bool_false){
					oss to_add;
					int coef = abs(isl_val_get_num_si(v));
					if(j!=0){
						if(coef != 1) to_add << coef << " * ";
						to_add << dim_name.get(j-1);
					}else{
						to_add << coef;
					}
					right_term << "(" << (vals_are_same_sign(dim_coef, v) ? "-" : "") << to_add.str() << ") + ";
				}
			}
			
			if(!right_term.str().empty()){
				string term = "(" + delete_last(right_term.str(), 3) + ")";
				if(one_elem_is_not_zero(mat, i, end_out, cols)){//if div are presents
					ISL::Val div_coef = isl_mat_get_element_val(mat, i, first_elem_is_not_zero(mat, i, end_out, cols));
					//term = "(InKS_Modulo(" + term + ", " + isl_val_to_str(div_coef) + "))";
					term = "((-" + term + ") % " + isl_val_to_str(div_coef) + ")";
				}
				if(isl_val_abs_eq(dim_coef, isl_val_one(ctx)) == isl_bool_false)
					term = "(" + term + " / " + std::to_string(abs(isl_val_get_num_si(dim_coef))) + ")";
					
				computations[index - start_out] += term + " + ";
			}else{
				computations[index - start_out] += "0 + ";
			}
		}
	}
	for(size_t i=0; i<computations.size(); ++i)
			delete_last(computations[i], 3);
	
	if(param->skip_condition && !debug_mode){
		basic_condition = "true";
	}else{
		bset_to_C(isl_basic_map_domain(ISL::copy(bmap)), &basic_condition);
		basic_condition.erase(0, 4);
		basic_condition = (basic_condition == "()") ? "true" : basic_condition;
	}
	
	if(debug_mode)
		for(size_t i=0; i<computations.size(); ++i)
			p << "size_t " << dim_name.get(i+start_out-1) << " = " << computations[i] << ";\n";
	
	if(isl_basic_map_dim(bmap, isl_dim_out) > 0){
		p << "return " << Array_name << "[";
		for(int i=0; i<isl_basic_map_dim(bmap, isl_dim_out)-1; ++i){
			p << Array_name << "_mult_" << i+1 << " * ";
			if(debug_mode)
				p << isl_basic_map_get_dim_name(bmap, isl_dim_out, i) << " + ";
			else
				p << computations[i] << " + ";
		}
		
		if(debug_mode)
			p << isl_basic_map_get_dim_name(bmap, isl_dim_out, isl_basic_map_dim(bmap, isl_dim_out)-1) << "];";
		else
			p << computations.back() << "];";
	}else{
		p << "return " << Array_name << ";";
	}
	
	param->push({basic_condition, p.str()});
	return isl_stat_ok;
}

isl_stat map_to_C(ISL::Map map, void* user){
	std::vector<std::string> outname(isl_map_dim(map, isl_dim_out));
	std::vector<std::string> inname(isl_map_dim(map, isl_dim_in));
	const char* name = isl_map_get_tuple_name(map, isl_dim_out);
	std::string Array_name(name != NULL ? name : "");
	Map_to_C* Cmapping = (Map_to_C*)user;
			
	for(int i=0; i<isl_map_dim(map, isl_dim_out); ++i){
		outname[i] = "out_" + std::to_string(i); //get_map_dim_name(map, isl_dim_out, i, "out_");
		map = isl_map_set_dim_name(map, isl_dim_out, i, outname[i].c_str());
	}	
	for(int i=0; i<isl_map_dim(map, isl_dim_in); ++i){
		inname[i] = "in_" + std::to_string(i); //get_map_dim_name(map, isl_dim_in, i, "in_");
		map = isl_map_set_dim_name(map, isl_dim_in, i, inname[i].c_str());
	}
	
	isl_map_foreach_basic_map(map, bmap_to_C, Cmapping);
	
	return isl_stat_ok;
}

std::string union_map_to_C(ISL::UMap umap){
	oss r;
	umap = isl_union_map_coalesce(umap);
	bool skip_condition = union_map_has_single_bmap(umap);
	Map_to_C parenthesis_operator(skip_condition);
	
	if(!debug_mode && !skip_condition){
		ISL::Set dom = isl_set_from_union_set(isl_union_map_domain(ISL::copy(umap)));
		ISL::L_BSet ldom = isl_set_get_basic_set_list(dom);
		int nb_bset = isl_basic_set_list_size(ldom);
		if(nb_bset > 1){
			bool heavily_gisted = false;
			int nb_dim = isl_set_dim(dom, isl_dim_set);
			std::vector<ISL::BSet> sets_unconstraints(nb_bset);
			
			for(int i=0; i<nb_dim && !heavily_gisted; i++){
				for(int id_bset=0; id_bset<nb_bset; id_bset++)
					sets_unconstraints[id_bset] = isl_basic_set_drop_constraints_not_involving_dims(isl_basic_set_list_get_basic_set(ldom, id_bset), isl_dim_set, i, 1);
				
				bool no_inequalities = true;
				bool no_intersection = true;
				for(int id_bset1=0; id_bset1<nb_bset && no_inequalities && no_intersection; id_bset1++){
					ISL::BSet comp1 = sets_unconstraints[id_bset1];
					no_inequalities = !basic_set_has_inequality(comp1);
					for(int id_bset2=id_bset1+1; id_bset2<nb_bset && no_inequalities && no_intersection; id_bset2++){
						ISL::BSet inter = isl_basic_set_intersect(ISL::copy(comp1), ISL::copy(sets_unconstraints[id_bset2]));
						no_intersection = (isl_basic_set_is_empty(inter) == isl_bool_true);
					}
				}
				
				if(no_intersection && no_inequalities){ //no intersection between each condition AND No inequalities 
														//-> you can remove all conditions excepted the ones on the ith dimension
					ISL::L_Map maps = isl_union_map_get_map_list(umap);
					int nb_map = isl_map_list_size(maps);
					umap = empty_isl_union_map();
					for(int map=0; map<nb_map; map++){
						ISL::Map the_map = isl_map_list_get_map(maps, map);
						ISL::L_BMap bmaps = isl_map_get_basic_map_list(the_map);
						int nb_bmap = isl_basic_map_list_size(bmaps);
						for(int bmap=0; bmap<nb_bmap; bmap++){
							ISL::BMap the_bmap = isl_basic_map_list_get_basic_map(bmaps, bmap);
							
							//std::vector<ISL::Mat(*)(ISL::BSet, isl_dim_type, isl_dim_type, isl_dim_type, isl_dim_type, isl_dim_type)> mat_func;
							//mat_func.push_back(isl_basic_map_equalities_matrix);
							//mat_func.push_back(isl_basic_map_inequalities_matrix);
							int start_param = 1;
							int end_param = isl_basic_map_dim(the_bmap, isl_dim_param) + start_param;
							int start_in = end_param;
							int end_in = start_in + isl_basic_map_dim(the_bmap, isl_dim_in);
							int start_out = end_in;
							int end_out = start_out + isl_basic_map_dim(the_bmap, isl_dim_out);
							int ith_dim = start_in + i;
							
							ISL::Mat eq = isl_basic_map_equalities_matrix(the_bmap, isl_dim_cst, isl_dim_param, isl_dim_in, isl_dim_out, isl_dim_div);
							int rows = isl_mat_rows(eq);
							int cols = isl_mat_cols(eq);
							for(int row=0; row<rows; ++row){
								//no constraint on the dim to keep AND no constraint on the out dimension
								if(elem_is_zero(eq, row, ith_dim) && ! one_elem_is_not_zero(eq, row, start_out, end_out)){
									for(int col=0; col<cols; ++col)
										eq = isl_mat_set_element_si(eq, row, col, 0);
								}
							}
							
							//Remove all inequalities constraints
							ISL::Mat ineq = isl_basic_map_inequalities_matrix(the_bmap, isl_dim_cst, isl_dim_param, isl_dim_in, isl_dim_out, isl_dim_div);
							rows = isl_mat_rows(ineq);
							cols = isl_mat_cols(ineq);
							for(int row=0; row<rows; ++row)
								for(int col=0; col<cols; ++col)
									ineq = isl_mat_set_element_si(ineq, row, col, 0);
							
							the_bmap = isl_basic_map_from_constraint_matrices(isl_basic_map_get_space(the_bmap), eq, ineq, isl_dim_cst, isl_dim_param, isl_dim_in, isl_dim_out, isl_dim_div);
							umap = isl_union_map_union(umap, isl_union_map_from_basic_map(the_bmap));
						}
					}
					heavily_gisted = true;
				}
			}
			
			if(! heavily_gisted){
				for(int i=0; i<nb_dim; i++){
					for(int id_bset=0; id_bset<nb_bset; id_bset++)
						sets_unconstraints[id_bset] = isl_basic_set_drop_constraints_not_involving_dims(isl_basic_set_list_get_basic_set(ldom, id_bset), isl_dim_set, i, 1);
				
					ISL::BSet inter = ISL::copy(sets_unconstraints[0]);
					for(int id_bset=1; id_bset<nb_bset; id_bset++)
						inter = isl_basic_set_intersect(inter, ISL::copy(sets_unconstraints[id_bset]));
					
					bool can_gist = true;
					for(int id_bset=0; id_bset<nb_bset && can_gist; id_bset++)
						can_gist = (isl_basic_set_is_equal(inter, sets_unconstraints[id_bset]) == isl_bool_true);
					
					if(can_gist) //complete intersection between the inter condition and all the bset -> can remove this condition
						umap = isl_union_map_gist_domain(umap, isl_union_set_from_basic_set(inter));					
				}
			}
		}
	}
	
	isl_union_map_foreach_map(umap, map_to_C, &parenthesis_operator);
	size_t nb_op = parenthesis_operator.c_str.size();
	
	if(debug_mode || nb_op > 1)
		r << "if(" << parenthesis_operator.c_str[0].condition << "){\n";
	
	r <<  parenthesis_operator.c_str[0].val << "\n";
	if(debug_mode || nb_op > 1)
		r << "}";
	
	for(size_t i=1; i<nb_op-1; i++){ //else contains the last "val"
		r << "else if(" << parenthesis_operator.c_str[i].condition << "){\n";
			r << parenthesis_operator.c_str[i].val << "\n";
		r << "}";
	}
	
	if(nb_op > 1){
		if(debug_mode)
			r << "else if(" << parenthesis_operator.c_str.back().condition << "){\n";
		else
			r << "else{\n";
		
		r << parenthesis_operator.c_str.back().val << "\n";
		r << "}";
	}
	if(debug_mode){
		r << "else{\n";
			r << "\tstd::cout << \"Error on mapping, line \" << __LINE__ << std::endl;" << "\n";
		r << "}";
	}
	
	return r.str();
}

std::string get_map_dim_name(ISL::Map m, isl_dim_type t, int dim, std::string prefix){
	const char* name = isl_map_get_dim_name(m, t, dim);
	std::ostringstream r;
	
	if(name == NULL) 	r << prefix << dim;
	else				r << prefix << name;
	
	return r.str();
}

isl_stat aff_to_C(ISL::Set set, ISL::Aff wf, void* user){
	
	VC_from_isl* allocate = (VC_from_isl*)user; /*0 -> condition / 1-> size*/
	ISL::Printer p_fold = alloc_isl_printer();
	std::string condition;
	p_fold = isl_printer_print_aff(p_fold, wf);
	
	isl_set_foreach_basic_set(isl_set_coalesce(set), bset_to_C, &condition);
	condition.erase(0, 4);
	if(condition == "()") condition = "(true)";	
	
	allocate->push_back({"("+condition+")", std::string(isl_printer_get_str(p_fold))});
	return isl_stat_ok;
}

VC_from_isl pw_aff_to_C(__isl_take ISL::PWAff pw_aff){
	VC_from_isl size;
	isl_pw_aff_foreach_piece(isl_pw_aff_coalesce(pw_aff), aff_to_C, &size);
	return size;
}

std::vector<VC_from_isl> multi_pw_aff_to_C(__isl_take ISL::M_PWAff multi_pw_aff){
	uint nb_dim = isl_multi_pw_aff_dim(multi_pw_aff, isl_dim_set);
	std::vector<VC_from_isl> sizes;
	VC_from_isl tmp_size;
	
	for(uint i=0; i<nb_dim; i++){
		tmp_size = pw_aff_to_C(isl_pw_aff_coalesce(isl_multi_pw_aff_get_pw_aff(multi_pw_aff, i)));
		sizes.push_back(std::move(tmp_size));
	}
	
	return sizes;
}

void vsize_from_isl_pw_aff(std::vector<std::string>& vsize, __isl_take ISL::PWAff pw_aff){
	oss size;
	VC_from_isl tmp_size = pw_aff_to_C(pw_aff);
	for(const auto& parse_aff : tmp_size)
			size << "(" << parse_aff.condition << ") ? " << parse_aff.val << " : ";
	vsize.push_back("(" + size.str() + "0)");
}

void vsize_from_isl_multi_pw_aff(std::vector<std::string>& vsize, __isl_take ISL::M_PWAff multi_pw_aff){
	std::vector<VC_from_isl> tmp_size = multi_pw_aff_to_C(multi_pw_aff);
	for(const auto& vparse_aff : tmp_size){
		oss size;
		for(const auto& parse_aff : vparse_aff)
			size << "(" << parse_aff.condition << ") ? " << parse_aff.val << " : ";
		vsize.push_back("(" + size.str() + "0)");
	}
}

std::string parameter_to_str(ISL::Set set, std::string message){
	oss r;
	std::string condition;
	isl_set_foreach_basic_set(set, bset_to_C, &condition);
	condition.erase(0, 4);
	if(! condition.empty()){
		if(! message.empty()) r << "\t" << message << endl;
		r << condition << endl;
	}
	
	return r.str();
}

bool bounded_set(ISL::Set set, size_t& i){
	size_t dim_size = isl_set_dim(set, isl_dim_set);
	
	for(i=0; i<dim_size; i++)
		if(! dim_bounded_set(set, i))
			return false;
	
	return true;
}

bool dim_bounded_set(ISL::Set set, size_t i){
	return (isl_set_dim_has_upper_bound(set, isl_dim_set, i) == isl_bool_true) && (isl_set_dim_has_lower_bound(set, isl_dim_set, i) == isl_bool_true);
}

std::string map_to_cpp(ISL::Map map){
	uint dim_out = isl_map_dim(map, isl_dim_out);
	uint dim_in = isl_map_dim(map, isl_dim_in);
	oss Cfunc;
	
	Cfunc << "size_t map_to_cpp(";
	for(uint i=0; i<dim_in; ++i)
		Cfunc << "size_t " << isl_map_get_dim_name(map, isl_dim_in, i) << add_coma_if(i, dim_in);
	Cfunc << "){" << endl;
	
	
	
	Cfunc << "return to_index(";
	for(uint i=0; i<dim_out; ++i)
		Cfunc << isl_map_get_dim_name(map, isl_dim_in, i) << add_coma_if(i, dim_in);
	Cfunc << ");" << endl;
	
	Cfunc << endl << "}" << endl;
	return Cfunc.str();
}

std::vector<VC_from_isl> set_component_to_string(__isl_keep ISL::Set set){
	std::vector<VC_from_isl> sizes;
	VC_from_isl tmp_size;
	
	for(int i=0; i<isl_set_dim(set, isl_dim_set); ++i){
		tmp_size = pw_aff_to_C(isl_set_dim_max(ISL::copy(set), i));
		sizes.push_back(std::move(tmp_size));
	}
	
	return sizes;
}

void name_map_in_from_set(ISL::Map& map, ISL::Set set){
	if(isl_map_dim(map, isl_dim_in) != isl_set_dim(set, isl_dim_set)){
		cerr << "*** Error: cannot name a map from a set that has not the same dimension number." << endl;
		exit(EXIT_FAILURE);
	}
	for(int i=0; i<isl_map_dim(map, isl_dim_in); ++i)
		map = isl_map_set_dim_id(map, isl_dim_in, i, isl_id_alloc(ctx, isl_set_get_dim_name(set, isl_dim_set, i), NULL));
}

ISL::USet union_set_complement(ISL::USet u){
	ISL::USet r = empty_isl_union_set();
	isl_union_set_foreach_set(u, [](ISL::Set set, void* user)->isl_stat{
		ISL::USet* r = (ISL::USet*)user;
		*r = isl_union_set_union(*r, isl_union_set_from_set(isl_set_complement(set)));
		return isl_stat_ok;
	}
	, &r);
	return isl_union_set_coalesce(r);
}

ISL::LSpace lspace_from_map(ISL::Map m){
	return isl_local_space_from_space(isl_map_get_space(m));
}

ISL::LSpace lspace_from_set(ISL::Set s){
	return isl_local_space_from_space(isl_set_get_space(s));
}

ISL::Set empty_isl_set(int dim){
	return isl_set_empty(isl_space_set_alloc(ctx, 0, dim));
}

ISL::Map empty_isl_map(int dim_in, int dim_out){
	return isl_map_empty(isl_space_alloc(ctx, 0, dim_in, dim_out));
}

ISL::USet empty_isl_union_set(){
	return isl_union_set_from_set(empty_isl_set());
}

ISL::UMap empty_isl_union_map(){
	return isl_union_map_from_map(empty_isl_map());
}

ISL::Set universe_isl_param_set(){
	return isl_set_universe(isl_space_params_alloc(ctx, 0));
}

ISL::Set universe_isl_set(int dim){
	return isl_set_universe(isl_space_set_alloc(ctx, 0, dim));
}

ISL::Map universe_isl_map(int dim_in, int dim_out){
	return isl_map_universe(isl_space_alloc(ctx, 0, dim_in, dim_out));
}

ISL::Map drop_param_constraint(ISL::Map map){
	ISL::Map m1 = isl_map_drop_constraints_not_involving_dims(ISL::copy(map), isl_dim_in, 0, isl_map_dim(map, isl_dim_in));
	ISL::Map m2 = isl_map_drop_constraints_not_involving_dims(map, isl_dim_out, 0, isl_map_dim(map, isl_dim_out));
	return isl_map_coalesce(isl_map_intersect(m1, m2));
}

ISL::Set drop_param_constraint(ISL::Set set){
	return isl_set_coalesce(isl_set_drop_constraints_not_involving_dims(set, isl_dim_set, 0, isl_set_dim(set, isl_dim_set)));
}

ISL::UMap drop_param_constraint(ISL::UMap umap){
	ISL::L_Map lmap = isl_union_map_get_map_list(umap);
	int nbmaps = isl_map_list_size(lmap);
	ISL::UMap r = empty_isl_union_map();
	
	for(int i=0; i<nbmaps; i++)
		r = isl_union_map_union(r, isl_union_map_from_map(drop_param_constraint(isl_map_list_get_map(lmap, i))));
	
	return isl_union_map_coalesce(r);
}

ISL::USet drop_param_constraint(ISL::USet uset){
	ISL::L_Set lset = isl_union_set_get_set_list(uset);
	int nbsets = isl_set_list_size(lset);
	ISL::USet r = empty_isl_union_set();
	
	for(int i=0; i<nbsets; i++)
		r = isl_union_set_union(r, isl_union_set_from_set(drop_param_constraint(isl_set_list_get_set(lset, i))));
	
	return isl_union_set_coalesce(r);
}

bool union_map_has_single_bmap(ISL::UMap umap){
	if(isl_union_map_n_map(umap) == 1){
		ISL::Map m = isl_map_from_union_map(ISL::copy(umap));
		bool r = isl_map_n_basic_map(m) == 1;
		ISL::free(m);
		return r;
	}
	
	return false;
}

bool union_set_has_single_bset(ISL::USet uset){
	if(isl_union_set_n_set(uset) == 1){
		ISL::Set s = isl_set_from_union_set(ISL::copy(uset));
		bool r = isl_set_n_basic_set(s) == 1;
		ISL::free(s);
		return r;
	}
	
	return false;
}

void date_union_map_n_lex_gt_0(ISL::UMap& um, int size, int start1){
	date_union_map_n_apply(um, [=] (ISL::Map& map) {
		for(int size_equality=0; size_equality<size; size_equality++){
			ISL::Constraint c = isl_constraint_alloc_inequality(lspace_from_map(map));
			c = isl_constraint_set_coefficient_si(c, isl_dim_in, start1+size_equality, 1);
			map = isl_map_add_constraint(map, c);
		}
	});
}

void date_union_map_n_lex_lt(ISL::UMap& um, int size, int start1, int start2){
	ISL::Set s = isl_set_from_union_set(isl_union_map_domain(ISL::copy(um)));
	ISL::Map m = isl_map_from_domain(isl_set_universe(isl_set_get_space(s)));
	date_map_n_lex_lt(m, size, start1, start2);
	
	um = isl_union_map_intersect_domain(um, isl_union_set_from_set(isl_map_domain(m)));
	ISL::free(s);
}

void date_union_map_n_lex_le(ISL::UMap& um, int size, int start1, int start2){
	date_union_map_n_apply(um, [=] (ISL::Map& map) { date_map_n_lex_le(map, size, start1, start2);});
}

void date_union_map_n_equality(ISL::UMap& um, int size, int start1, int start2){
	date_union_map_n_apply(um, [=] (ISL::Map& map) { date_map_n_equality(map, size, start1, start2);});
}

//{ [a, b, c, a', b', c'] : a' > a; [a, b, c, a' = a, b', c'] : b' > b; [a, b, c, a' = a, b' = b, c'] : c' > c }
void date_map_n_lex_lt(ISL::Map& m, int size, int start1, int start2){
	ISL::Map mres = isl_map_empty(isl_map_get_space(m));
	for(int size_equality=0; size_equality<size; size_equality++){
		ISL::Map mwork = ISL::copy(m);
		for(int j=0; j<size_equality; j++){
			ISL::Constraint c = isl_constraint_alloc_equality(lspace_from_map(mwork));
			c = isl_constraint_set_coefficient_si(c, isl_dim_in, start1+j, 1);
			c = isl_constraint_set_coefficient_si(c, isl_dim_in, start2+j, -1);
			mwork = isl_map_add_constraint(mwork, c);
		}
		ISL::Constraint c = isl_constraint_alloc_inequality(lspace_from_map(mwork));
		c = isl_constraint_set_coefficient_si(c, isl_dim_in, start1+size_equality, -1);
		c = isl_constraint_set_coefficient_si(c, isl_dim_in, start2+size_equality, 1);
		c = isl_constraint_set_constant_si(c, -1);
		mwork = isl_map_add_constraint(mwork, c);
		mres = isl_map_union(mwork, mres);
	}
	ISL::free(m);
	m = mres;
}

//map_n_lex_lt + { [a, b, c, a' = a, b' = b, c' = c] }
void date_map_n_lex_le(ISL::Map& m, int size, int start1, int start2){
	ISL::Map mwork = ISL::copy(m);
	date_map_n_lex_lt(m, size, start1, start2);
	
	for(int size_equality=0; size_equality<size; size_equality++){
		ISL::Constraint c = isl_constraint_alloc_equality(lspace_from_map(mwork));
		c = isl_constraint_set_coefficient_si(c, isl_dim_in, start1+size_equality, 1);
		c = isl_constraint_set_coefficient_si(c, isl_dim_in, start2+size_equality, -1);
		mwork = isl_map_add_constraint(mwork, c);
	}
	
	m = isl_map_union(m, mwork);
}

void date_map_n_equality(ISL::Map& m, int size, int start1, int start2){
	for(int i=0; i<size; i++){
		ISL::Constraint c = isl_constraint_alloc_equality(lspace_from_map(m));
		c = isl_constraint_set_coefficient_si(c, isl_dim_in, i+start1, 1);
		c = isl_constraint_set_coefficient_si(c, isl_dim_in, i+start2, -1);
		m = isl_map_add_constraint(m, c);
	}
}

void date_union_map_project(ISL::UMap& um, int size, int start){
	date_union_map_n_apply(um, [=] (ISL::Map& map) {
		map = isl_map_project_out(map, isl_dim_in, start, size);
	});
}

void date_union_map_insert_dims(ISL::UMap& um, int size, int start){
	if(start == -1){
		date_union_map_n_apply(um, [=] (ISL::Map& map) {
				map = isl_map_add_dims(map, isl_dim_in, size);
		});
	}else{
		date_union_map_n_apply(um, [=] (ISL::Map& map) {
			map = isl_map_insert_dims(map, isl_dim_in, start, size);
		});
	}
}

void date_union_map_self_cartesian_produit(ISL::UMap& um, int sch_size, int start){
	if(sch_size == -1){
		ISL::Set s = isl_set_from_union_set(isl_union_map_domain(ISL::copy(um)));
		sch_size = isl_set_dim(s, isl_dim_set);
		ISL::free(s);
	}
	ISL::UMap e1 = ISL::copy(um);
	ISL::UMap e2 = um;
	date_union_map_insert_dims(e1, sch_size, start+sch_size);
	date_union_map_insert_dims(e2, sch_size, start);

	um = isl_union_map_coalesce(isl_union_map_intersect(e1, e2));
}

//um = [time1] -> [[L->P]]
//um2 = [time2] -> [X] : X = L or P
//res = [time1 Inter time2] -> [[L->P]] : L inter X or P inter X
void date_union_map_intersect_range_wrapped(ISL::UMap& um, ISL::UMap& um2){
	/*
	date_union_map_n_apply(um, [&] (ISL::Map& lpmap){
		ISL::Map LogPhy = isl_set_unwrap(isl_map_range(ISL::copy(lpmap)));
		ISL::Set Phy = isl_map_range(ISL::copy(LogPhy));
		ISL::UMap umap = isl_union_map_intersect_range(ISL::copy(um2), isl_union_set_from_set(isl_set_universe(isl_set_get_space(Phy))));
		
		if(isl_union_map_is_empty(umap) == isl_bool_false){
			ISL::Map map = isl_map_from_union_map(umap);
			lpmap = isl_map_intersect_domain(lpmap, isl_map_domain(ISL::copy(map)));
			lpmap = isl_map_intersect_range(lpmap, isl_map_wrap(isl_map_intersect_range(LogPhy, isl_map_range(map))));
		}else{
			ISL::Space lpmap_spc = isl_map_get_space(lpmap);
			ISL::free(lpmap);
			lpmap = isl_map_empty(lpmap_spc);
			ISL::free(umap);
			ISL::free(LogPhy);
		}
		ISL::free(Phy);
	});
	*/
	um = isl_union_map_coalesce(isl_union_map_intersect_range_factor_range(um, um2));
}

void date_union_map_intersect_domain_wrapped(ISL::UMap& um, ISL::UMap& um2){
	/*
	date_union_map_n_apply(um, [&um2] (ISL::Map& lpmap){
		ISL::Map LogPhy = isl_set_unwrap(isl_map_range(ISL::copy(lpmap)));
		ISL::Set Log = isl_map_domain(ISL::copy(LogPhy));
		ISL::UMap umap = isl_union_map_intersect_range(ISL::copy(um2), isl_union_set_from_set(isl_set_universe(isl_set_get_space(Log))));
		
		if(isl_union_map_is_empty(umap) == isl_bool_false){
			ISL::Map map = isl_map_from_union_map(umap);
			lpmap = isl_map_intersect_domain(lpmap, isl_map_domain(ISL::copy(map)));
			lpmap = isl_map_intersect_range(lpmap, isl_map_wrap(isl_map_intersect_domain(LogPhy, isl_map_range(map))));
		}else{
			ISL::Space lpmap_spc = isl_map_get_space(lpmap);
			ISL::free(lpmap);
			lpmap = isl_map_empty(lpmap_spc);
			ISL::free(umap);
			ISL::free(LogPhy);
		}
		ISL::free(Log);
	});
	*/
	um = isl_union_map_coalesce(isl_union_map_range_reverse(isl_union_map_intersect_range_factor_range(isl_union_map_range_reverse(um), um2)));
}


bool basic_set_has_inequality(ISL::BSet bset){
	return (isl_mat_rows(isl_basic_set_inequalities_matrix(bset, isl_dim_param, isl_dim_set, isl_dim_cst, isl_dim_div)) != 0);
}


bool elem_is_zero(ISL::Mat mat, int row, int col){
	return (isl_val_is_zero(isl_mat_get_element_val(mat, row, col)) == isl_bool_true);
}

void add_dims_to_0(ISL::Set& s, int nb){
	int size = isl_set_dim(s, isl_dim_set);
	s = isl_set_add_dims(s, isl_dim_set, nb);
	for(int i=0; i<nb; i++){
		ISL::Constraint c = isl_constraint_alloc_equality(isl_local_space_from_space(isl_set_get_space(s)));
		c = isl_constraint_set_coefficient_si(c, isl_dim_set, i+size, 1);
		s = isl_set_add_constraint(s, c);
	}
}

void add_dims_to_0(ISL::Map& m, isl_dim_type t, int nb){
	int size = isl_map_dim(m, t);
	m = isl_map_add_dims(m, t, nb);
	for(int i=0; i<nb; i++){
		ISL::Constraint c = isl_constraint_alloc_equality(isl_local_space_from_space(isl_map_get_space(m)));
		c = isl_constraint_set_coefficient_si(c, t, i+size, 1);
		m = isl_map_add_constraint(m, c);
	}
}

void set_add_value_to_dim(ISL::Set& set, int v, int dim){
	ISL::M_Aff ma = isl_multi_aff_identity(isl_space_map_from_set(isl_set_get_space(set)));
	ISL::Aff aff = isl_aff_val_on_domain(lspace_from_set(set), isl_val_int_from_si(ctx, v));
	ma = isl_multi_aff_set_aff(ma, dim, isl_aff_sub(isl_multi_aff_get_aff(ma, dim), aff));
	set = isl_set_preimage_multi_aff(set, ma);
}

void set_mul_value_to_dim(ISL::Set& set, int v, int dim){
	ISL::M_Aff ma = isl_multi_aff_identity(isl_space_map_from_set(isl_set_get_space(set)));
	ISL::Aff aff = isl_aff_val_on_domain(lspace_from_set(set), isl_val_int_from_si(ctx, v));
	ma = isl_multi_aff_set_aff(ma, dim, isl_aff_div(isl_multi_aff_get_aff(ma, dim), aff));
	set = isl_set_preimage_multi_aff(set, ma);
}

void set_add_value_to_dim(ISL::Set& set, ISL::PWAff v, int dim){
	ISL::PWM_Aff ma = isl_pw_multi_aff_identity(isl_space_map_from_set(isl_set_get_space(set)));
	ISL::PWAff valigned = isl_pw_aff_add_dims(ISL::copy(v), isl_dim_in, isl_set_dim(set, isl_dim_set));
	ma = isl_pw_multi_aff_set_pw_aff(ma, dim, isl_pw_aff_sub(isl_pw_multi_aff_get_pw_aff(ma, dim), valigned));
	set = isl_set_preimage_pw_multi_aff(set, ma);
}

void set_set_dim_to_const(ISL::Set& set, int cst, int dim){
	ISL::Constraint c = isl_constraint_alloc_equality(lspace_from_set(set));
	c = isl_constraint_set_coefficient_si(c, isl_dim_set, dim, -1);
	c = isl_constraint_set_constant_si(c, cst);
	set = isl_set_drop_constraints_involving_dims(set, isl_dim_set, dim, 1);
	set = isl_set_add_constraint(set, c);
}

void map_dim_equal(ISL::Map& m, int in, int out, isl_dim_type tin, isl_dim_type tout){
	ISL::Constraint c = isl_constraint_alloc_equality(lspace_from_map(m));
	c = isl_constraint_set_coefficient_si(c, tin, in, -1);
	c = isl_constraint_set_coefficient_si(c, tout, out, 1);
	m = isl_map_add_constraint(m, c);
}
	
ISL::Set set_identity_to_param(ISL::Space spc){
	int nb_dim = isl_space_dim(spc, isl_dim_set);
	int offset_param = isl_space_dim(spc, isl_dim_param) - nb_dim;
	ISL::Set s = isl_set_universe(spc);
	
	if(offset_param < 0){
		std::cerr << "***Error: cannot create identity set if there are less parameters than input dimension." << std::endl;
		std::cerr << "\tSet: " << isl_set_to_str(s) << std::endl;
		exit(EXIT_FAILURE);
	}
	
	for(int i=0; i<nb_dim; ++i){
		ISL::Constraint c = isl_constraint_alloc_equality(isl_local_space_from_space(isl_set_get_space(s)));
		c = isl_constraint_set_coefficient_si(c, isl_dim_set, i, 1);
		c = isl_constraint_set_coefficient_si(c, isl_dim_param, i+offset_param, -1);
		s = isl_set_add_constraint(s, c);
	}
	
	return s;
}

ISL::Map map_identity_to_param(ISL::Space spc, isl_dim_type type){
	int nb_dim = isl_space_dim(spc, type);
	int offset_param = isl_space_dim(spc, isl_dim_param) - nb_dim;
	ISL::Map m = isl_map_universe(spc);
	
	if(offset_param < 0){
		std::cerr << "***Error: cannot create identity map if there are less parameters than input dimension." << std::endl;
		std::cerr << "\tMap: " << isl_map_to_str(m) << std::endl;	
		exit(EXIT_FAILURE);
	}
	
	for(int i=0; i<nb_dim; ++i)
		map_dim_equal(m, i, i+offset_param, type, isl_dim_param);
	
	return m;
}

bool one_elem_is_zero(ISL::Mat mat, int row, int start, int end){
	for(int col=start; col<end; col++)
		if(elem_is_zero(mat, row, col))
			return true;
	return false;
}

bool elem_is_not_zero(ISL::Mat mat, int row, int col){
	return ! elem_is_zero(mat, row, col);
}

bool one_elem_is_not_zero(ISL::Mat mat, int row, int start, int end){
	for(int col=start; col<end; col++)
		if(elem_is_not_zero(mat, row, col))
			return true;
	return false;
}

bool only_one_elem_is_not_zero(ISL::Mat mat, int row, int start, int end){
	for(int col=start; col<end; col++)
		if(elem_is_not_zero(mat, row, col))
			for(int col2=col+1; col2<end; col2++){
				if(elem_is_not_zero(mat, row, col))
					return false;
				return true;
			}
	return false;
}

int first_elem_is_not_zero(ISL::Mat mat, int row, int start, int end){
	for(int col=start; col<end; col++)
		if(elem_is_not_zero(mat, row, col))
			return col;
	return -1;
}

bool has_constraint(ISL::Mat mat, int row, int col1, int col2){
	return (elem_is_not_zero(mat, row, col1) && elem_is_not_zero(mat, row, col2));
}

bool vals_are_same_sign(ISL::Val v1, ISL::Val v2){
	return ((isl_val_sgn(v1) * isl_val_sgn(v2)) == 1);
}