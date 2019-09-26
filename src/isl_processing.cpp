#include <algorithm>
#include <cstdlib>
#include <sstream>

#include <Declarations/Auto_buffer_decl_t.hpp>
#include <Declarations/Function_Decl_t.hpp>
#include <Function_application_t.hpp>
#include <Codes/Auto_buffer_t.hpp>
#include <isl_processing.hpp>
#include <const_globals.hpp>
#include <Array_Map.hpp>
#include <isl_utils.hpp>
#include <Implem_t.hpp>
#include <Input_t.hpp>
#include <globals.hpp>
#include <Array.hpp>
#include <utils.hpp>

using vstring = std::vector<std::string>;
using oss = std::ostringstream;
using namespace std;

ISL::Set valid_parameters;
ISL::UMap RI;
ISL::UMap RO;
ISL::USet SI;
ISL::USet SO;
ISL::UMap Rmap;
ISL::UMap RmapInvFT;
ISL::UMap Proximity;
ISL::UMap Copy_out;
ISL::UMap Copy_in;
ISL::USet Kernel_set;
ISL::USet Kernel_target_set;
ISL::USet Target_set;

void Print_ISL_Object(){
	cerr << "Input relation_________" << endl << isl_union_map_to_str(RI) << endl << endl;
	cerr << "Output relation________" << endl << isl_union_map_to_str(RO) << endl << endl;
	cerr << "Input set______________" << endl << isl_union_set_to_str(SI) << endl << endl;
	cerr << "Output set_____________" << endl << isl_union_set_to_str(SO) << endl << endl;
	cerr << "Proximity relation_____" << endl << isl_union_map_to_str(Proximity) << endl << endl;
	cerr << "Order relation_________" << endl << isl_union_map_to_str(Rmap) << endl << endl;
	cerr << "Domain set_____________" << endl << isl_union_set_to_str(Target_set) << endl;
}

void apply_valid_parameters(){
	RI = isl_union_map_intersect_params(RI, ISL::copy(valid_parameters));
	RO = isl_union_map_intersect_params(RO, ISL::copy(valid_parameters));
	Rmap = isl_union_map_intersect_params(Rmap, ISL::copy(valid_parameters));
	Proximity = isl_union_map_intersect_params(Proximity, ISL::copy(valid_parameters));
	Target_set = isl_union_set_intersect_params(Target_set, ISL::copy(valid_parameters));
	SI = isl_union_set_coalesce(isl_union_set_intersect_params(SI, ISL::copy(valid_parameters)));
	SO = isl_union_set_coalesce(isl_union_set_intersect_params(SO, ISL::copy(valid_parameters)));
	
	RI = isl_union_map_coalesce(isl_union_map_gist_params(RI, ISL::copy(valid_parameters)));
	RO = isl_union_map_coalesce(isl_union_map_gist_params(RO, ISL::copy(valid_parameters)));
	Rmap = isl_union_map_coalesce(isl_union_map_gist_params(Rmap, ISL::copy(valid_parameters)));
	Proximity = isl_union_map_coalesce(isl_union_map_gist_params(Proximity, ISL::copy(valid_parameters)));
	Target_set = isl_union_set_coalesce(isl_union_set_gist_params(Target_set, ISL::copy(valid_parameters)));
	Gen.Arrays->intersect_valid_parameter(valid_parameters);
}

void Initialize_ISL_Object (){
	if(Gen.no_generate()) exit(EXIT_FAILURE);
	
	clock_t st = clock();
	Gen.initDep();
	Functions.init(Gen.Vin, Gen.vars);
	Copy_out = Gen.createOutputMap();
	Copy_in = Gen.createInputMap();
	
	Kernel_set = Gen.createApplySet();
	Kernel_set = isl_union_set_union(Kernel_set, isl_union_map_domain(ISL::copy(Copy_out)));
	Kernel_set = isl_union_set_union(Kernel_set, isl_union_map_domain(ISL::copy(Copy_in)));
	RI = Gen.createInRelation();
	RI = isl_union_map_union(RI, ISL::copy(Copy_out));
	RO = Gen.createOutRelation();
	RO = isl_union_map_union(RO, ISL::copy(Copy_in));

	SI = isl_union_set_coalesce(isl_union_set_apply(ISL::copy(Kernel_set), ISL::copy(RI)));
	SO = isl_union_set_coalesce(isl_union_set_apply(ISL::copy(Kernel_set), ISL::copy(RO)));
	Kernel_target_set = isl_union_set_coalesce(isl_union_set_union(isl_union_map_domain(ISL::copy(Copy_out)), isl_union_map_domain(ISL::copy(Copy_in))));
	print_elapsed_time("* ISL objects initialization: ", st);
	
	if ( verbosity ) {
		cerr << "Kernel set_____________" << endl << ISL::str(Kernel_set) << endl << endl;
		cerr << "Kernel target set______" << endl << ISL::str(Kernel_target_set) << endl << endl;
		cerr << "Original Input relation_________" << endl << ISL::str(RI) << endl << endl;
		cerr << "Original Output relation________" << endl << ISL::str(RO) << endl << endl;
		cerr << "Original Input set______________" << endl << ISL::str(SI) << endl << endl;
		cerr << "Original Output set_____________" << endl << ISL::str(SO) << endl << endl;
	}
	
	st = clock();
	Rmap = RmapAlloc(Proximity, RI, RO, SI, SO);
	print_elapsed_time("* Order relation: ", st);
	
	st = clock();
	RmapInvFT = RmapInvFTAlloc(Rmap, true);
	print_elapsed_time("* Transitive closure order relation: ", st);
	
	st = clock();
	Target_set = TargetAlloc(RmapInvFT, Kernel_target_set, RI, RO, SI, SO);
	print_elapsed_time("* Domain: ", st);
	
	st = clock();
	valid_parameters = Gen.array_isl_property(SI, SO);
	if(isl_set_is_empty(valid_parameters)) valid_parameters = isl_set_universe(isl_set_get_space(valid_parameters));
	valid_parameters = isl_set_intersect_params(valid_parameters, Gen.get_restrict());
	cout << parameter_to_str(valid_parameters, "Valid values for structuring data: ") << endl;
	apply_valid_parameters();
	print_elapsed_time("* Multidimensionnal logical arrays properties: ", st);
		
	if(verbosity) Print_ISL_Object();
}

ISL::UMap RmapAlloc(ISL::UMap& Prox_rel, ISL::UMap Read_rel, ISL::UMap Write_rel, ISL::USet Read_data, ISL::USet Write_data){
	ISL::USet dio = isl_union_set_coalesce(isl_union_set_intersect(ISL::copy(Read_data), ISL::copy(Write_data)));
	ISL::USet inDom = isl_union_set_coalesce(isl_union_set_apply(dio, isl_union_map_reverse(ISL::copy(Write_rel))));
	Prox_rel = isl_union_map_coalesce(isl_union_map_apply_range(ISL::copy(Write_rel), isl_union_map_reverse(ISL::copy(Read_rel))));
	return isl_union_map_coalesce(isl_union_map_intersect_domain(ISL::copy(Prox_rel), inDom));
}

ISL::UMap RmapInvFTAlloc(ISL::UMap Order_rel, bool fast){
	ISL::UMap RFTInv;
	isl_bool ex;
	
	if(fast && !no_fast){
		FILE* dump = fopen("dump.ikf", "r");
		if(dump == NULL){
			RFTInv = isl_union_map_reverse(isl_union_map_coalesce(isl_union_map_transitive_closure(ISL::copy(Order_rel), &ex)));
			dump = fopen("dump.ikf", "w");
			ISL::Printer p = isl_printer_to_file(ctx, dump);
			isl_printer_print_union_map(p, RFTInv);
			fclose(dump);
		}else{
			std::cerr << "***Warning: DUMP file was used to speed up the computation ! If you changed your PIA, you must remove this file.\n";
			RFTInv = isl_union_map_read_from_file(ctx, dump);
		}
	}else{
		return isl_union_map_reverse(isl_union_map_coalesce(isl_union_map_transitive_closure(ISL::copy(Order_rel), &ex)));
	}
	
	return RFTInv;
}

ISL::USet TargetAlloc(ISL::UMap Order_relInvFT, ISL::USet Last_kernels, ISL::UMap Read_rel, ISL::UMap Write_rel, ISL::USet& Read_data, ISL::USet& Write_data){
	ISL::USet Domain = isl_union_set_coalesce(isl_union_set_union(ISL::copy(Last_kernels), isl_union_set_apply(ISL::copy(Last_kernels), ISL::copy(Order_relInvFT))));
	Read_data = isl_union_set_coalesce(isl_union_set_apply(ISL::copy(Domain), ISL::copy(Read_rel)));
	Write_data = isl_union_set_coalesce(isl_union_set_apply(ISL::copy(Domain), ISL::copy(Write_rel)));
	
	return Domain;
}

ISL::Printer do_that(ISL::Printer p, ISL::Ast_POption opt, ISL::Ast_node node, void* user){
	
	Gen_sched_t* gs = (Gen_sched_t*)user;
	isl_ast_expr* user_expr = isl_ast_node_user_get_expr(node);
	string name = isl_id_get_name(isl_ast_expr_get_id(isl_ast_expr_get_op_arg(user_expr, 0)));
	gs->gen_code += isl_printer_get_str(p);
	oss args;
	
	if(name.find(Array::COPY_NAME) != string::npos){ /*Call copy kernel: InKS_Copy_out_ARRNAME*/
		vstring splited_name = split(name, "_");
		string arr_name;
		for(size_t i=3; i<splited_name.size(); ++i)
			arr_name += splited_name[i] + add_str_if(i, splited_name.size(), "_");
			
		if(splited_name[2] == "out"){
			args << "InOut_" << arr_name << ", " << arr_name;
		}else if(splited_name[2] == "in"){
			args << arr_name << ", " << "InOut_" << arr_name;
		}else{
			cerr << "*** Error: Copy neither in nor out" << endl;
			exit(EXIT_FAILURE);
		}
		args << ", " << Gen.get_params();
	}else{
		Function_application_t* dom = Gen.fa_from_instance_name(name);
		string param_array;
		if(dom != nullptr){
			name = dom->Func_ref->name;
			if(gs->l_to_p.empty()){
				param_array = dom->get_arr_arg();
			}else{
				for(uint i=0; i<dom->nb_array(); i++)
					param_array += gs->l_to_p.at(dom->get_arr_arg(i)) + ", ";
			}
		}else{
			Function_t& F = Functions.at(name);
			param_array = F.get_arr_arg();
		}
		
		size_t size = isl_ast_expr_get_op_n_arg(user_expr);
		if(size <= 1) 	args << delete_last(param_array, 2);
		else			args << param_array;
		
		for(size_t isl_ind = 1; isl_ind < size; isl_ind++)
			args << isl_ast_expr_to_C_str(isl_ast_expr_get_op_arg(user_expr, isl_ind)) << add_coma_if(isl_ind, size);
	}
	
	if(inline_mode) gs->gen_code += "#pragma forceinline recursive\n";
	
	gs->gen_code += name + "(" + args.str() + ");\n";	
	return isl_printer_flush(p);
}

void add_ivdep(Function_t& F, isl_ast_node* node, isl_ast_expr* Fcall, std::string& gen_code){
	isl_ast_expr* iterator = isl_ast_node_for_get_iterator(node);
	for(size_t i=0; i<F.dim_size(); i++){
		if(isl_ast_expr_is_equal(isl_ast_expr_get_op_arg(Fcall, 1+i), iterator) == isl_bool_true){
			if(! F.is_self_dependent(i)) gen_code += "#pragma ivdep\n";
			break;
		}
	}
}

ISL::Printer do_that_for(ISL::Printer p, ISL::Ast_POption opt, ISL::Ast_node node, void* user){
	ISL::Ast_node body = isl_ast_node_for_get_body(node);
	std::string* gen_code = (std::string*)user;
	
	*gen_code += isl_printer_get_str(p);
	p = isl_printer_flush(p);
	if(isl_ast_node_get_type(body) == isl_ast_node_user){
		
		isl_ast_expr* Fcall = isl_ast_node_user_get_expr(body);
		std::string name = isl_id_get_name(isl_ast_expr_get_id(isl_ast_expr_get_op_arg(Fcall, 0)));
		
		Function_Map::F_iterator F = Functions.find(name);
		if(F != Functions.end()){
			add_ivdep(F->second, node, Fcall, *gen_code);
		}else{
			Function_application_t* fa = Gen.Apply->func_from_instance(name);
			if(fa != nullptr){
				add_ivdep(*(fa->Func_ref), node, Fcall, *gen_code);
			}else{
				*gen_code += "#pragma ivdep\n";
			}
		}
	}
	
	return isl_ast_node_for_print(node, p, opt);
}

string parse_ast_node(ISL::Ast_node ast, const std::map<std::string, std::string>& l_to_p, ISL::Printer (*print_user)(ISL::Printer, ISL::Ast_POption, ISL::Ast_node, void*)){
	string gen_code;
	ISL::Printer printer = alloc_isl_printer();
	ISL::Ast_POption opt = isl_ast_print_options_alloc(ctx);
	
	Gen_sched_t gs(gen_code, l_to_p);
	opt = isl_ast_print_options_set_print_user(opt, print_user, &gs);
	opt = isl_ast_print_options_set_print_for(opt, do_that_for, &gen_code);
	printer = isl_ast_node_print(ast, printer, opt);
	
	gen_code += isl_printer_get_str(printer);
	return gen_code;
}

ISL::Ast_node Generate_Scheduling(ISL::UMap& Return_sched){
	return Generate_Scheduling(Return_sched, Target_set, valid_parameters, Rmap, Proximity);
}

ISL::Ast_node Generate_Scheduling(ISL::UMap& Return_sched, ISL::USet Domain, ISL::Set Context, ISL::UMap Order_relation, ISL::UMap Prox_relation){
	
	isl_schedule_constraints* sch_const = isl_schedule_constraints_on_domain(Domain);
	if(Order_relation != nullptr)	sch_const = isl_schedule_constraints_set_validity(sch_const, Order_relation);
	if(Prox_relation != nullptr)	sch_const = isl_schedule_constraints_set_proximity(sch_const, Prox_relation);
	ISL::Sched sch = isl_schedule_constraints_compute_schedule(sch_const);
	Return_sched = isl_schedule_get_map(sch);

	ISL::Ast_build ast_build;
	if(Context != nullptr && isl_set_is_empty(Context) == false)
		ast_build = isl_ast_build_from_context(ISL::copy(Context));
	else
		ast_build = isl_ast_build_alloc(ctx);
	ISL::Ast_node ast = isl_ast_build_node_from_schedule(ast_build, sch);
	
	return ast;
}

void Generate_Mapping(Implem_t& implem, ISL::UMap Sch_map){
	ISL::USet data_used = isl_union_set_coalesce(isl_union_set_union(SI, SO));
	if (verbosity) ISL::print(data_used, "Data used________\n");
		
	auto gen_mapping_set = [&] (ISL::USet Set){
		ISL::L_Set lset = isl_union_set_get_set_list(Set);
		int nb = isl_set_list_size(lset);
		for(int i=0; i<nb; ++i){
			ISL::Set s = isl_set_list_get_set(lset, i);
			Array& logical_ref = Gen.Arrays->at(isl_set_get_tuple_name(s));
			implem.push_decl(new Auto_buffer_decl_t(logical_ref, implem.const_params, s, valid_parameters));
			implem.push_code(new Auto_buffer_t(logical_ref, implem.const_params));
		}
	};
	
	//1) Compute the memory mapping
	ISL::USet Delta = compute_conflict(Sch_map);
	gen_mapping_set(Delta);
	
	//2) Compute the naive mapping for arrays that couldn't be allocated smartly (typically cause all values are needed)
	ISL::USet data_not_allocated = isl_union_set_subtract(data_used, isl_union_set_universe(Delta));
	gen_mapping_set(data_not_allocated);
}

ISL::USet compute_conflict(ISL::UMap Sch_map){
    ISL::UMap SchRev = isl_union_map_reverse(ISL::copy(Sch_map));
	uint max_dim = isl_set_dim(isl_set_from_union_set(isl_union_map_range(Sch_map)), isl_dim_set);
	
	ISL::Space PSpc = isl_space_set_alloc(ctx, 0, max_dim);
	ISL::UMap PrevEQ = isl_union_map_from_map(isl_map_lex_le(ISL::copy(PSpc)));
    ISL::UMap Prev = isl_union_map_from_map(isl_map_lex_lt(PSpc));
    
	ISL::UMap WriteBeforeT = isl_union_map_coalesce(isl_union_map_apply_range(isl_union_map_reverse(PrevEQ), ISL::copy(SchRev)));
	WriteBeforeT = isl_union_map_coalesce(isl_union_map_apply_range(WriteBeforeT, ISL::copy(RO)));
	ISL::UMap ReadAfterT = isl_union_map_coalesce(isl_union_map_apply_range(Prev, ISL::copy(SchRev)));
	ReadAfterT = isl_union_map_coalesce(isl_union_map_apply_range(ReadAfterT, ISL::copy(RI)));
	ISL::UMap WriteAtT = isl_union_map_apply_range(SchRev, ISL::copy(RO));
	ISL::UMap Live = isl_union_map_coalesce(isl_union_map_intersect(WriteBeforeT, ReadAfterT));
	ISL::UMap Conflict = isl_union_map_coalesce(isl_union_map_apply_range(isl_union_map_reverse(Live), WriteAtT));
	ISL::USet Delta = isl_union_set_coalesce(isl_union_map_deltas(Conflict));
	
	
	ISL::L_Set lsets = isl_union_set_get_set_list(Delta);
	int nbset = isl_set_list_size(lsets);
	for(int i=0; i<nbset; i++){
		ISL::L_BSet lbsets = isl_set_get_basic_set_list(isl_set_list_get_set(lsets, i));
		int nbbset = isl_basic_set_list_size(lbsets);
		for(int j=0; j<nbbset; j++){
			if(isl_basic_set_dim(isl_basic_set_list_get_basic_set(lbsets, j), isl_dim_div) > 0){
				cerr << "**Warning: Deltas contains existentially quantified variables that must be removed (maybe gives incorrect values !)\n";
				Delta = isl_union_set_coalesce(isl_union_set_remove_divs(Delta));
				i = nbset;
				break;
			}
		}
	}
	
	if ( verbosity ) cerr << "Deltas_____________" << endl << isl_union_set_to_str(Delta) << endl << endl;
	
	
	return Delta;
}

ISL::Printer generate_inout(ISL::Printer p, ISL::Ast_POption opt, ISL::Ast_node node, void* user){

	isl_ast_expr* user_expr = isl_ast_node_user_get_expr(node);
	string name = isl_id_get_name(isl_ast_expr_get_id(isl_ast_expr_get_op_arg(user_expr, 0)));
	string* InorOut = (string*)user;
	*InorOut += isl_printer_get_str(p);
	
	size_t size = Gen.Arrays->size(name);
	if(size > 0){
		string args;
		for(size_t isl_ind = 1; isl_ind <= size; isl_ind++)
			args += isl_ast_expr_to_C_str(isl_ast_expr_get_op_arg(user_expr, isl_ind)) + add_coma_if(isl_ind-1, size);
		args = "(" + args + ")";
		
		*InorOut += "arr_1" + args + " = arr_2" + args + ";\n";
	}else{
		*InorOut += "arr_1 = arr_2;\n";
	}
	
	return isl_printer_flush(p);
}

void generate_copy_fct(ISL::Map copy_map, Implem_t& implem, const V_Input_t& Vin, ISL::Set context){
	string copy_name(isl_map_get_tuple_name(copy_map, isl_dim_in));
	std::string prototype = "(T1& arr_1, T2& arr_2, " + Vin.get_params(true) + ")";
	std::string body;
	
	ISL::Printer printer = alloc_isl_printer();
	ISL::USet uset = isl_union_set_from_set(isl_map_range(copy_map));
	isl_schedule_constraints* sch_const = isl_schedule_constraints_on_domain(uset);
	ISL::Sched sch = isl_schedule_constraints_compute_schedule(sch_const);
	
	ISL::Ast_build ast_build = isl_ast_build_from_context(ISL::copy(context));
	ISL::Ast_node ast = isl_ast_build_node_from_schedule(ast_build, sch);
	ISL::Ast_POption opt = isl_ast_print_options_alloc(ctx);
	opt = isl_ast_print_options_set_print_user(opt, generate_inout, &body);
	opt = isl_ast_print_options_set_print_for(opt, do_that_for, &body);

	printer = isl_ast_node_print(ast, printer, opt);
	
	body += isl_printer_get_str(printer);
	
	implem.push_decl(new Function_Decl_t({"T1", "T2"}, "void", copy_name, prototype, body));
}

void Generate_InOut_copy(Implem_t& implem){
	ISL::UMap Cmap = isl_union_map_union(ISL::copy(Copy_in), ISL::copy(Copy_out));
	
	ISL::L_Map lmaps = isl_union_map_get_map_list(Cmap);
	int nb = isl_map_list_size(lmaps);
	for(int i=0; i<nb; ++i)
		generate_copy_fct(isl_map_list_get_map(lmaps, i), implem, Gen.Vin, valid_parameters);
}
