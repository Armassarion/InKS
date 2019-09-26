#ifndef INKS_ISL_PROCESSING_HPP__
#define INKS_ISL_PROCESSING_HPP__

#include <string>
#include <vector>
#include <map>

#include <isl_cpp_type.hpp>

class V_Input_t;
class Array_Map;
class Implem_t;

extern ISL::Set valid_parameters;
extern ISL::UMap RI;
extern ISL::UMap RO;
extern ISL::USet SI;
extern ISL::USet SO;
extern ISL::UMap Rmap;
extern ISL::UMap RmapInvFT;
extern ISL::UMap Proximity;
extern ISL::UMap Copy_in;
extern ISL::UMap Copy_out;
extern ISL::USet Kernel_set;
extern ISL::USet Kernel_target_set; /*First target on kernel space*/
extern ISL::USet Target_set;

void apply_valid_parameters();

void Print_ISL_Object();

void Initialize_ISL_Object();

ISL::UMap RmapAlloc(ISL::UMap& Prox_rel, ISL::UMap Read_rel, ISL::UMap Write_rel, ISL::USet Read_data, ISL::USet Write_data);

ISL::UMap RmapInvFTAlloc(ISL::UMap Order_rel, bool fast=false);

ISL::USet TargetAlloc(ISL::UMap Order_relInvFT, ISL::USet Last_kernels, ISL::UMap Read_rel, ISL::UMap Write_rel, ISL::USet& Read_data, ISL::USet& Write_data);

ISL::Printer do_that(ISL::Printer p, ISL::Ast_POption opt, ISL::Ast_node node, void* user);

ISL::Printer do_that_for(ISL::Printer p, ISL::Ast_POption opt, ISL::Ast_node node, void* user);

struct Gen_sched_t{
	
	std::string& gen_code;
	//logical_id must be replaced by the physical_id
	const std::map<std::string, std::string>& l_to_p;
	
	Gen_sched_t(std::string& _g, const std::map<std::string, std::string>& _lp) : 
		gen_code(_g),
		l_to_p(_lp)
	{}
};
	
ISL::Ast_node Generate_Scheduling(ISL::UMap& Return_sched);

ISL::Ast_node Generate_Scheduling(ISL::UMap& Return_sched, ISL::USet Domain, ISL::Set Context=nullptr, ISL::UMap Order_relation=nullptr, ISL::UMap Prox_relation=nullptr);
		
std::string parse_ast_node(ISL::Ast_node ast, const std::map<std::string, std::string>& l_to_p = std::map<std::string, std::string>(), ISL::Printer (*print_user)(ISL::Printer, ISL::Ast_POption, ISL::Ast_node, void*) = do_that);
	
void generate_copy_fct(ISL::Map copy_map, Implem_t& implem, const V_Input_t& Vin, ISL::Set context);
	
void Generate_InOut_copy(Implem_t& implem);

void Generate_Mapping(Implem_t& vbuff, ISL::UMap Sch_map);

ISL::USet compute_conflict(ISL::UMap Sch_map);
	
#endif
