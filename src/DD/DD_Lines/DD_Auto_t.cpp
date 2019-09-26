#include <iostream>

#include <Scope_exception_t.hpp>
#include <List_named_poly_t.hpp>
#include <isl_processing.hpp>
#include <Poly_scope_t.hpp>
#include <const_globals.hpp>
#include <DD_Update_t.hpp>
#include <Set_Poly_t.hpp>
#include <DD_Auto_t.hpp>
#include <isl_utils.hpp>
#include <DD_Mem_t.hpp>
#include <Implem_t.hpp>
#include <DD_Gen_t.hpp>
#include <globals.hpp>
#include <utils.hpp>
DD_Auto_t::DD_Auto_t(int _lineno, List_named_poly_t* _up) :
	DD_Update_t(_lineno, _up)
{}

void DD_Auto_t::print_kernel() const{
	std::cout << "Kernel choosed automatically.\n";
}
/*
void DD_Auto_t::generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context){
	
	ISL::USet cObjective = isl_union_set_coalesce(isl_union_set_intersect_params(ISL::copy(Objective), ISL::copy(context)));
	ISL::USet cSO = isl_union_set_coalesce(isl_union_set_intersect_params(ISL::copy(SO), ISL::copy(context)));
	ISL::USet cSI = isl_union_set_coalesce(isl_union_set_intersect_params(ISL::copy(SI), ISL::copy(context)));
	ISL::UMap cRO = isl_union_map_coalesce(isl_union_map_intersect_params(ISL::copy(RO), ISL::copy(context)));
	ISL::UMap cRI = isl_union_map_coalesce(isl_union_map_intersect_params(ISL::copy(RI), ISL::copy(context)));
	
	ISL::USet Last_kernels = isl_union_set_apply(ISL::copy(cObjective), isl_union_map_reverse(ISL::copy(cRO)));
	
	ISL::UMap LRmap = isl_union_map_intersect_domain(ISL::copy(Rmap), isl_union_map_domain(ISL::copy(cRO)));
	ISL::UMap LRInvFT = isl_union_map_intersect_range(ISL::copy(RmapInvFT), isl_union_map_domain(ISL::copy(cRO)));
	ISL::USet Kernel_set = TargetAlloc(LRInvFT, Last_kernels, cRI, cRO, cSI, cSO);
	ISL::USet write_area = isl_union_set_apply(ISL::copy(Kernel_set), ISL::copy(cRO));
	ISL::USet read_area = isl_union_set_apply(ISL::copy(Kernel_set), ISL::copy(cRI));
	
	this->test(cRO, cRI, write_area, read_area, context);
	this->generate_mapping(implem, events, context);
	this->generate_scheduling(implem, Kernel_set, context, LRmap, Proximity);	
	
}
*/

void DD_Auto_t::scope(DD_Scope_t& scope_object){
	
	my_scope = scope_object;
	{
		Objective = parsed_area->createSet(scope_object.get_const(), scope_object.get_list_vars());
		if(verbosity) std::cerr << "Auto Update: " << ISL::str(SO) << std::endl;
	}
	
	//To improve !! It says that it read/write EVERYTHING even though it is obviously false
	//Should probably add a new Read/Write action and subtract to them the thing that are not read/updated
	
	RO = Functions.getOut_Applied();
	RI = Functions.getIn_Applied();
	SI = isl_union_map_range(ISL::copy(RI));
	SO = isl_union_map_range(ISL::copy(RO));
	
	this->add_read_write(scope_object);
	
	scope_object.print("Auto", lineno);
}