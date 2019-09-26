#include <List_named_poly_t.hpp>
#include <Poly_scope_t.hpp>
#include <Named_Poly_t.hpp>
#include <List_var_t.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>


List_named_poly_t::List_named_poly_t(Named_Poly_t* np){
	polys.push_back(np);
}
	
List_named_poly_t* List_named_poly_t::push(Named_Poly_t* np){
	polys.push_back(np);
	return this;
}

ISL::USet List_named_poly_t::createSet(const V_Input_t& Vin, List_var_t& lvars){
	Poly_scope_t polyscope(Vin);
	ISL::USet rset = empty_isl_union_set();
	ISL::Set set = isl_set_universe(polyscope.param_space());
	
	for(Set_Poly_t* p : polys){
		p = lvars.get_poly(p);
		p->scope(polyscope);
		rset = isl_union_set_union(rset, isl_union_set_from_set(p->createSet(ISL::copy(set))));
	}
		
	return isl_union_set_coalesce(rset);
}