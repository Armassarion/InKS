#include <V_Modification_t.hpp>
#include <isl_utils.hpp>
#include <List_t.hpp>
#include <utility>
#include <utils.hpp>
#include <const_globals.hpp>

using std::unique_ptr;

void V_Modification_t::push_map(DD_Line_t* _line, ISL::Map map){
	modifs.push_back(unique_ptr<Modification_t>(new AddMap_t(_line, map)));
}

void V_Modification_t::push_unmap(DD_Line_t* _line, ISL::Set unmap){
	modifs.push_back(unique_ptr<Modification_t>(new AddUnmap_t(_line, unmap)));
}

void V_Modification_t::push_read(DD_Line_t* _line, ISL::USet read){
	modifs.push_back(unique_ptr<Modification_t>(new AddRead_t(_line, read)));
}

void V_Modification_t::push_write(DD_Line_t* _line, ISL::USet write){
	modifs.push_back(unique_ptr<Modification_t>(new AddWrite_t(_line, write)));
}

void V_Modification_t::push_readphy(DD_Line_t* _line, ISL::USet read){
	modifs.push_back(unique_ptr<Modification_t>(new AddReadPhy_t(_line, read)));
}

void V_Modification_t::push_writephy(DD_Line_t* _line, ISL::USet write){
	modifs.push_back(unique_ptr<Modification_t>(new AddWritePhy_t(_line, write)));
}

Time_type_t& V_Modification_t::push_addfor(DD_Line_t* _line, ISL::Set addfor, int nb_action){
	AddFor_t* f = new AddFor_t(_line, addfor, nb_action);
	modifs.push_back(unique_ptr<Modification_t>(f));
	return f->original_time;
}
void V_Modification_t::push_delfor(DD_Line_t* _line, ISL::Set delfor, Time_type_t& t){
	modifs.push_back(unique_ptr<Modification_t>(new DelFor_t(_line, delfor, t)));
}

void V_Modification_t::operator+=(V_Modification_t& vm){
	modifs.insert(modifs.end(), std::make_move_iterator(vm.begin()), std::make_move_iterator(vm.end()));
}

Time_Event_t V_Modification_t::get_SchEvent(ISL::Set context, ISL::USet logical_areas){
	clock_t st = clock();
	int sch_size;
	int nb_vars = isl_set_dim(context, isl_dim_param);
	ISL::Set original_sch = this->get_original_sch(sch_size);
		
	Modification_t::Time_to_action_t time_to_action;
	
	time_to_action[Modification_t::Type::Map] = empty_isl_union_map();
	time_to_action[Modification_t::Type::Unmap] = empty_isl_union_map();
	time_to_action[Modification_t::Type::Write] = empty_isl_union_map();
	time_to_action[Modification_t::Type::Read] = empty_isl_union_map();
	time_to_action[Modification_t::Type::WritePhy] = empty_isl_union_map();
	time_to_action[Modification_t::Type::ReadPhy] = empty_isl_union_map();

	int dim = 0;
	Modification_t::time_type_t time = Modification_t::init_time();
	//set_set_dim_to_const(original_sch, 1, dim);
	//time++;
	for(auto& m : modifs)
		m->sch_to_modif(original_sch, time, dim, time_to_action);

	//Project the for parameters
	for(auto& pair : time_to_action){
		if(isl_union_map_is_empty(pair.second) == isl_bool_false){
			int nbparam = isl_union_map_dim(pair.second, isl_dim_param);
			pair.second = isl_union_map_coalesce(isl_union_map_project_out(pair.second, isl_dim_param, nb_vars, nbparam-nb_vars));
		}
	}
	print_elapsed_time("° Gathering all PSO actions: ", st);
	
	return Time_Event_t(sch_size, time.val+1	, time_to_action[Modification_t::Type::Map]
												, time_to_action[Modification_t::Type::Unmap]
												, time_to_action[Modification_t::Type::Read]
												, time_to_action[Modification_t::Type::Write]
												, time_to_action[Modification_t::Type::ReadPhy]
												, time_to_action[Modification_t::Type::WritePhy], logical_areas, context);
}

int V_Modification_t::sch_size() const{
	int r = 1;
	int tmp = 1;
	for(const auto& m : modifs){
		tmp += m->sch_size();
		if(r < tmp) r = tmp;
	}

	return r;
}

ISL::Set V_Modification_t::get_original_sch(int& sch_size) const{
	sch_size = this->sch_size();
	ISL::Set sch = universe_isl_set(sch_size);
	
	for(int i=0; i<sch_size; i++){
		ISL::Constraint c = isl_constraint_alloc_equality(lspace_from_set(sch));
		c = isl_constraint_set_coefficient_si(c, isl_dim_set, i, 1);
		sch = isl_set_add_constraint(sch, c);
	}

	return sch;
}

void V_Modification_t::print() const{
	for(const auto& m : modifs){
		m->print();
		std::cout << "\n";
	}
}

std::vector<std::unique_ptr<Modification_t>>::iterator V_Modification_t::begin(){
	return modifs.begin();
}
	
std::vector<std::unique_ptr<Modification_t>>::iterator V_Modification_t::end(){
	return modifs.end();
}