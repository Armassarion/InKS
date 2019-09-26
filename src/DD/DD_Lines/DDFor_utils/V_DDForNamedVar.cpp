#include <DDFor_utils/Options/DDForOptions_t.hpp>
#include <DDFor_utils/V_DDForNamedVar.hpp>
#include <Poly_scope_t.hpp>
#include <DD_Scope_t.hpp>
#include <isl_utils.hpp>
#include <Point_t.hpp>
#include <List_t.hpp>

const char* V_DDForNamedVar::DD_For_kernel_name = "For_Kernel";
	
DDForNamedVar::DDForNamedVar(DDForOptions_t* _opt, const char* _id, Constraint_t* _c) : 
	options(_opt),
	id(_id),
	c(_c)
{}

V_DDForNamedVar::V_DDForNamedVar(DDForNamedVar* _var){
	vars.push_back(_var);
}

V_DDForNamedVar* V_DDForNamedVar::push(DDForNamedVar* _var){
	vars.push_back(_var);
	return this;
}

Point_t* V_DDForNamedVar::get_point() const{
	Point_t* p = new Point_t();
	for(const auto& v : vars)
		p->push(v->c);
	return p;
}

ISL::Map V_DDForNamedVar::get_order_map(DD_Scope_t& scope_object) const{
	ISL::Map order_map;
	const V_Input_t& vin = scope_object.get_const();
	Poly_scope_t polyscope(vin);
	
	order_map = isl_map_identity(vin.map_space(this->size(), this->size()));
	ISL::Set domain = scope_object.get_poly(this->get_point())->createSet(polyscope);
	order_map = isl_map_intersect_domain(order_map, domain);
	order_map = isl_map_set_tuple_name(order_map, isl_dim_in, DD_For_kernel_name);
	
	return isl_map_coalesce(order_map);
}

ISL::Map V_DDForNamedVar::get_order_map_param(DD_Scope_t& scope_object) const{
	
	ISL::Map order_map = this->get_order_map(scope_object);
	int nb_param_original = isl_map_dim(order_map, isl_dim_param);
	ISL::Map order_param = isl_map_add_dims(ISL::copy(order_map), isl_dim_param, this->size());
	this->to_list()->name_map(order_param, isl_dim_param, nb_param_original);
	
	for(uint i=0; i<this->size(); i++){
		ISL::Constraint c = isl_constraint_alloc_equality(isl_local_space_from_space(isl_map_get_space(order_param)));
		c = isl_constraint_set_coefficient_si(c, isl_dim_param, scope_object.const_size() + i, 1);
		c = isl_constraint_set_coefficient_si(c, isl_dim_in, i, -1);
		order_param = isl_map_add_constraint(order_param, c);
		
		//Loop MUST be positiv at the moment
		//c = isl_constraint_alloc_inequality(lspace_from_map(order_param));
		//c = isl_constraint_set_coefficient_si(c, isl_dim_param, my_scope.const_size() + i, 1);
		//order_param = isl_map_add_constraint(order_param, c);
	}

	return isl_map_coalesce(order_param);
}

ISL::L_Id V_DDForNamedVar::to_id_list() const{
	return this->to_list()->to_id_list();
}

List_t* V_DDForNamedVar::to_list() const{
	List_t* l = new List_t();
	for(const auto& v : vars)
		l->push(v->id);
	return l;
}

std::string V_DDForNamedVar::add_str_before(const std::string& iterator_id){
	for(auto& v : vars){
		if(v->id == iterator_id){
			return v->options->add_str_before();
			break;
		}
	}
	
	std::cerr << "Unknown iterator \"" << iterator_id << "\" !!\n";
	exit(0);
}

size_t V_DDForNamedVar::size() const{
	return vars.size();
}