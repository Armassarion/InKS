#include <Modification_t.hpp>
#include <const_globals.hpp>
#include <DD_Line_t.hpp>
#include <isl_utils.hpp>
#include <iostream>

using std::cout;
using std::endl;

Modification_t::Modification_t(DD_Line_t* _line) : 
	line(_line),
	mysch(nullptr)
{}

int Modification_t::get_lineno() const{
	return line->lineno;
}

int Modification_t::sch_size() const{
	return 0;
}

ISL::UMap& Modification_t::sch_to_modif_base(ISL::Set& sch, time_type_t& time, int& dim, Time_to_action_t& tta){
	ISL::UMap& the_map = tta[this->modif_type()];
	mysch = ISL::copy(sch);
	time.set_dim_to_time(mysch, dim);
	this->line->set_time(this->modif_type(), mysch);
	
	time++;
	return the_map;
}

AddMap_t::AddMap_t(DD_Line_t* _line, ISL::Map _m) : 
	Modification_t(_line),
	m(ISL::copy(_m))
{}

void AddMap_t::sch_to_modif(ISL::Set& sch, time_type_t& time, int& dim, Time_to_action_t& tta){
	ISL::UMap& the_map = sch_to_modif_base(sch, time, dim, tta);
	ISL::UMap r = isl_union_map_coalesce(isl_union_map_from_map(isl_map_from_domain_and_range(ISL::copy(mysch), isl_map_wrap(ISL::copy(m)))));
	the_map = isl_union_map_union(the_map, r);
}
	
Modification_t::Type AddMap_t::modif_type() const{
	return Type::Map;
}

void AddMap_t::print() const{
	cout << "Map -> " << ISL::str(mysch) << " at " << ISL::str(m) << endl;
}
	
AddUnmap_t::AddUnmap_t(DD_Line_t* _line, ISL::Set _u) : 
	Modification_t(_line),
	unmap(ISL::copy(_u))
{}

void AddUnmap_t::sch_to_modif(ISL::Set& sch, time_type_t& time, int& dim, Time_to_action_t& tta){
	ISL::UMap& the_map = sch_to_modif_base(sch, time, dim, tta);
	ISL::UMap r = isl_union_map_coalesce(isl_union_map_from_domain_and_range(isl_union_set_from_set(ISL::copy(mysch)), isl_union_set_from_set(ISL::copy(unmap))));
	the_map = isl_union_map_union(the_map, r);
}
	
Modification_t::Type AddUnmap_t::modif_type() const{
	return Type::Unmap;
}

void AddUnmap_t::print() const{
	cout << "Unmap -> " << ISL::str(mysch) << " at " << ISL::str(unmap) << endl;
}
	
AddRead_t::AddRead_t(DD_Line_t* _line, ISL::USet _set) : 
	Modification_t(_line),
	set(ISL::copy(_set))
{}

void AddRead_t::sch_to_modif(ISL::Set& sch, time_type_t& time, int& dim, Time_to_action_t& tta){
	ISL::UMap& the_map = sch_to_modif_base(sch, time, dim, tta);
	ISL::UMap r = isl_union_map_coalesce(isl_union_map_from_domain_and_range(isl_union_set_from_set(ISL::copy(mysch)), ISL::copy(set)));
	the_map = isl_union_map_union(the_map, r);
}
	
Modification_t::Type AddRead_t::modif_type() const{
	return Type::Read;
}

const char* AddRead_t::modif_print() const{
	return "Read -> ";
}

void AddRead_t::print() const{
	cout << this->modif_print() << ISL::str(mysch) << " at " << ISL::str(set) << endl;
}
	
AddWrite_t::AddWrite_t(DD_Line_t* _line, ISL::USet _set) : 
	AddRead_t(_line, _set)
{}

Modification_t::Type AddWrite_t::modif_type() const{
	return Type::Write;
}

const char* AddWrite_t::modif_print() const{
	return "Write -> ";
}
	
AddReadPhy_t::AddReadPhy_t(DD_Line_t* _line, ISL::USet _set) : 
	AddRead_t(_line, _set)
{}

Modification_t::Type AddReadPhy_t::modif_type() const{
	return Type::ReadPhy;
}

const char* AddReadPhy_t::modif_print() const{
	return "ReadPhy -> ";
}

AddWritePhy_t::AddWritePhy_t(DD_Line_t* _line, ISL::USet _set) : 
	AddRead_t(_line, _set)
{}

Modification_t::Type AddWritePhy_t::modif_type() const{
	return Type::WritePhy;
}

const char* AddWritePhy_t::modif_print() const{
	return "WritePhy -> ";
}
	
AddFor_t::AddFor_t(DD_Line_t* _line, ISL::Set s, int _nb_action) : 
	Modification_t(_line),
	vars(ISL::copy(s)),
	nb_action(_nb_action)
{}
	
int AddFor_t::sch_size() const{
	return isl_set_dim(vars, isl_dim_set) + 1;
}

void AddFor_t::sch_to_modif(ISL::Set& sch, time_type_t& time, int& dim, Time_to_action_t& tta){
	const int nbvarssch = isl_set_dim(sch, isl_dim_set);
	const int nbparam = isl_set_dim(vars, isl_dim_param);
	const int nbvars = this->sch_size();
	ISL::Set align_vars;
	
	original_time.val = time.val;
	//original_time.mul_val = ISL::copy(time.mul_val);

	//Align_vars
	{
		align_vars = isl_set_insert_dims(ISL::copy(vars), isl_dim_set, 0, dim + 1);
		align_vars = isl_set_add_dims(align_vars, isl_dim_set, nbvarssch - nbvars - dim);
		//align_vars = isl_set_add_dims(align_vars, isl_dim_set, nbvarssch - nbvars - dim - 1);
	}
	time.set_dim_to_time(sch, dim);
	sch = isl_set_drop_constraints_involving_dims(sch, isl_dim_set, dim+1, nbvars);
	sch = isl_set_intersect(sch, align_vars);
	dim += nbvars;
	//set_mul_value_to_dim(sch, nb_action, dim);
	//set_set_value_to_dim(sch, nb_action, dim);
	set_set_dim_to_const(sch, 0, dim);
	time.val = 1;
	
	/*
	time.mul_val = isl_set_dim_max(isl_set_project_out(ISL::copy(vars), isl_dim_param, nbparam-nbvars, nbvars), nbvars-1);
	ISL::PWAff one = isl_pw_aff_val_on_domain(isl_pw_aff_domain(ISL::copy(time.mul_val)), isl_val_one(ctx));
	time.mul_val = isl_pw_aff_add(time.mul_val, one);
	*/
}

Modification_t::Type AddFor_t::modif_type() const{
	return Type::AddFor;
}

void AddFor_t::print() const{
	cout << "AddFor -> " << ISL::str(vars) << endl;
}

DelFor_t::DelFor_t(DD_Line_t* _line, ISL::Set s, Time_type_t& t) : 
	Modification_t(_line),
	vars(ISL::copy(s)),
	original_time(t)
{}
	
int DelFor_t::sch_size() const{
	return -isl_set_dim(vars, isl_dim_set) - 1;
}
	
void DelFor_t::sch_to_modif(ISL::Set& sch, time_type_t& time, int& dim, Time_to_action_t& tta){
	dim += this->sch_size();
	for(int i=0; i < (-this->sch_size()); i++)
		set_set_dim_to_const(sch, 0, dim+i);
		//set_set_dim_to_const(sch, 0, dim+i+1);
		
		
	//Set const value to 0
	//set_add_value_to_dim(sch, -original_time.val, dim);
	time.val = original_time.val + 1;
	//time.mul_val = ISL::copy(original_time.mul_val);
}

Modification_t::Type DelFor_t::modif_type() const{
	return Type::DelFor;
}

void DelFor_t::print() const{
	cout << "DelFor -> " << ISL::str(vars) << endl;
}