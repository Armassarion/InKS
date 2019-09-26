#include <iostream>

#include <Declarations/Buffer_class_decl_t.hpp>
#include <Codes/Buffer_class_t.hpp>
#include <Scope_exception_t.hpp>
#include <List_named_poly_t.hpp>
#include <isl_processing.hpp>
#include <Poly_scope_t.hpp>
#include <const_globals.hpp>
#include <Codes/Code_t.hpp>
#include <DD_Update_t.hpp>
#include <Set_Poly_t.hpp>
#include <isl_utils.hpp>
#include <DD_Mem_t.hpp>
#include <Implem_t.hpp>
#include <DD_Gen_t.hpp>
#include <globals.hpp>
#include <Array.hpp>

std::map<std::string, uint> DD_Update_t::allocator_id;

DD_Update_t::DD_Update_t(int _lineno, List_named_poly_t* _up, List_t* _kids) :
	DD_Line_t(_lineno),
	parsed_area(_up),
	kernel_ids(_kids)
{}

DD_Update_t::DD_Update_t(int _lineno, List_named_poly_t* _up) :
	DD_Line_t(_lineno),
	parsed_area(_up),
	kernel_ids(new List_t())
{}

void DD_Update_t::print_kernel() const{
	kernel_ids->print();
}

void DD_Update_t::generate_scheduling(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
	ISL::UMap sched;
		
	ISL::USet cObjective = isl_union_set_coalesce(isl_union_set_intersect_params(ISL::copy(Objective), ISL::copy(context)));
	ISL::USet cSO = isl_union_set_coalesce(isl_union_set_intersect_params(ISL::copy(SO), ISL::copy(context)));
	ISL::USet cSI = isl_union_set_coalesce(isl_union_set_intersect_params(ISL::copy(SI), ISL::copy(context)));
	ISL::UMap cRO = isl_union_map_coalesce(isl_union_map_intersect_params(ISL::copy(RO), ISL::copy(context)));
	ISL::UMap cRI = isl_union_map_coalesce(isl_union_map_intersect_params(ISL::copy(RI), ISL::copy(context)));
	
	ISL::UMap LProx;
	ISL::USet Last_kernels = isl_union_set_apply(ISL::copy(cObjective), isl_union_map_reverse(ISL::copy(cRO)));
	ISL::UMap LRmap = RmapAlloc(LProx, cRI, cRO, cSI, cSO);
	ISL::UMap LRFTInv = RmapInvFTAlloc(LRmap);
	ISL::USet Kernel_set = TargetAlloc(LRFTInv, Last_kernels, cRI, cRO, cSI, cSO);

	ISL::Ast_node ast = Generate_Scheduling(sched, Kernel_set, context, LRmap, LProx);
	implem.push_code(parse_ast_node(ast, my_scope.get_l_to_p()));
}

void DD_Update_t::generate_mapping(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
	bool bprint = false;// (std::string(kernel_ids->get(0)) == std::string("advection_eta4"));
	
	if(bprint) ISL::print(time[Modification_t::Type::Write]);
	ISL::UMap map_used = events.map_from_time_to_write(time[Modification_t::Type::Write]);
	
	if(bprint) ISL::print(map_used);
	if(time.count(Modification_t::Type::Read)){ // also some read
		ISL::UMap read = events.map_from_time_to_read(time[Modification_t::Type::Read]);
		if(bprint) ISL::print(read);
		map_used = isl_union_map_coalesce(isl_union_map_union(map_used, read));
	}
	if(bprint) ISL::print(map_used, "total");
	
	foreach_set(isl_union_map_domain(ISL::copy(map_used)), [&] (ISL::Set log_area){
		std::string lid(isl_set_get_tuple_name(log_area));
		ISL::UMap memmap = isl_union_map_coalesce(isl_union_map_gist_params(isl_union_map_intersect_domain(ISL::copy(map_used), isl_union_set_from_set(log_area)), ISL::copy(context)));
		
		if(isl_set_dim(log_area, isl_dim_set) > 0){ //not scalar
			std::string actual_lid(lid + "_" + std::to_string((allocator_id.count(lid) ? ++allocator_id[lid] : allocator_id[lid] = 1)));
			my_scope.set_l_to_p(lid, actual_lid);

			implem.push_decl(new Buffer_class_decl_t(actual_lid, my_scope.array(lid).type, my_scope.get_const(), memmap));
			implem.push_code(new Buffer_class_t(actual_lid, memmap, my_scope.get_const()));
		}else{
			ISL::Set phy = isl_set_from_union_set(isl_union_map_range(memmap));
			my_scope.set_l_to_p(lid, isl_set_get_tuple_name(phy));
		}
	});
	
	if(bprint) std::cout << "\n\n";
}

void DD_Update_t::generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
	this->generate_mapping(implem, events, context);
	this->generate_scheduling(implem, events, context);	
}

void DD_Update_t::check(const Time_Event_t& events){
	
}

void DD_Update_t::scope(DD_Scope_t& scope_object){
		
	my_scope = scope_object;
	{
		SO = parsed_area->createSet(scope_object.get_const(), scope_object.get_list_vars());
		Objective = ISL::copy(SO);
		if(verbosity) std::cerr << "Update: " << ISL::str(SO) << std::endl;
	}
	
	RO = Functions.getOut_Applied(*kernel_ids);
	RI = Functions.getIn_Applied(*kernel_ids);
	
	ISL::UMap SI_to_SO = isl_union_map_apply_range(isl_union_map_reverse(ISL::copy(RI)), ISL::copy(RO));
	SI = isl_union_set_apply(ISL::copy(SO), isl_union_map_reverse(ISL::copy(SI_to_SO)));
	
	this->add_read_write(scope_object);
	scope_object.print("Update", lineno);
}

void DD_Update_t::add_read_write(DD_Scope_t& scope_object){
	
	foreach_set(SO, [&] (ISL::Set set){
		std::string lid(isl_set_get_tuple_name(set));
		if(! scope_object.logical_contains(lid))
			throw Scope_exception_t(lineno, std::string("Logical array \"") + lid + std::string("\" does not exist.\n"));
	});
	
	try{
		if(isl_union_set_is_empty(SI) == isl_bool_false)
			scope_object.modifications.push_read(this, SI);
			
		scope_object.modifications.push_write(this, SO);
	}catch(Scope_exception_t& se){
		throw Scope_exception_t(lineno, se.what());
	}
}