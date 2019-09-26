#include <const_globals.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>
#include <List_t.hpp>
#include <utils.hpp>
#include <stdexcept>
#include <sstream>

using vstring = std::vector<std::string>;
using std::string;

List_t::List_t(){}

List_t::List_t(const List_t& _list){
	for(string s : _list)
		l.push_back(s);
}

List_t::List_t(string s){
	l.push_back(s);
}

List_t::List_t(char* s){
	l.push_back(string(s));
}

List_t::List_t(ISL::Set s, Add_param ap){
	std::vector<isl_dim_type> types({isl_dim_set});
	if(ap == T) types.insert(types.begin(), isl_dim_param);
	
	for(const auto& t : types){
		size_t nb_dim = isl_set_dim(s, t);
		for(size_t i=0; i<nb_dim; ++i){
			if(isl_set_get_dim_name(s, t, i) == NULL)
				throw std::runtime_error("***Error - List_t from S: empty dim " + std::to_string(i) + std::string(" -> ") + std::string(isl_set_to_str(s)));
			l.push_back(string(isl_set_get_dim_name(s, t, i)));
		}
	}
}

List_t::List_t(ISL::BSet bs, Add_param ap){
	std::vector<isl_dim_type> types({isl_dim_set});
	if(ap == T) types.insert(types.begin(), isl_dim_param);
	
	for(const auto& t : types){
		size_t nb_dim = isl_basic_set_dim(bs, t);
		for(size_t i=0; i<nb_dim; ++i){
			if(isl_basic_set_get_dim_name(bs, t, i) == NULL)
				throw std::runtime_error("***Error - List_t from BS: empty dim " + std::to_string(i) + std::string(" -> ") + std::string(isl_basic_set_to_str(bs)));
			l.push_back(string(isl_basic_set_get_dim_name(bs, t, i)));
		}
	}
}

List_t::List_t(ISL::Map m, Add_param ap){
	std::vector<isl_dim_type> types({isl_dim_in, isl_dim_out});
	if(ap == T) types.insert(types.begin(), isl_dim_param);
	
	for(const auto& t : types){
		size_t nb_dim = isl_map_dim(m, t);
		for(size_t i=0; i<nb_dim; ++i){
			if(isl_map_get_dim_name(m, t, i) == NULL)
				throw std::runtime_error("***Error - List_t from M: empty dim " + std::to_string(i) + std::string(" -> ") + std::string(isl_map_to_str(m)));
			l.push_back(string(isl_map_get_dim_name(m, t, i)));
		}
	}
}

List_t::List_t(ISL::BMap bm, Add_param ap){
	std::vector<isl_dim_type> types({isl_dim_in, isl_dim_out});
	if(ap == T) types.insert(types.begin(), isl_dim_param);
	
	for(const auto& t : types){
		size_t nb_dim = isl_basic_map_dim(bm, t);
		for(size_t i=0; i<nb_dim; ++i){
			if(isl_basic_map_get_dim_name(bm, t, i) == NULL)
				throw std::runtime_error("***Error - List_t from BM: empty dim " + std::to_string(i) + std::string(" -> ") + std::string(isl_basic_map_to_str(bm)));
			l.push_back(string(isl_basic_map_get_dim_name(bm, t, i)));
		}
	}
}

List_t* List_t::push(char* s){
	l.push_back(string(s));
	return this;
}

List_t* List_t::push(string s){
	l.push_back(s);
	return this;
}

size_t List_t::size() const{
	return l.size();
}

const char* List_t::get(int i) const{
	return l[i].c_str();
}

vstring::iterator List_t::begin(){
	return l.begin();
}

vstring::iterator List_t::end(){
	return l.end();
}

const vstring::const_iterator List_t::begin() const{
	return l.begin();
}

const vstring::const_iterator List_t::end() const{
	return l.end();
}
	
std::string List_t::fill_param() const{
	std::ostringstream args;
	for(size_t i=0; i<l.size(); i++)
		args << "const int " << l[i] << add_coma_if(i, l.size());
	return args.str();
}
	
ISL::Set& List_t::name_isl_set(ISL::Set& set) const {
	for(size_t i=0; i<l.size(); i++)
		set = isl_set_set_dim_id(set, isl_dim_set, i, isl_id_alloc(ctx, l[i].c_str(), NULL));
	return set;
}

ISL::Set List_t::create_dimension_set(const V_Input_t& Vin) const {
	ISL::Set s = isl_set_universe(Vin.set_space(size()));
	return name_isl_set(s);
}

ISL::Set List_t::create_dimension_set(ISL::Space spc) const {
	ISL::Set func_set = isl_set_universe(spc);
	name_isl_set(func_set);
	
	/*Add constraints such that DIMi = VARi*/
	ISL::LSpace ls = isl_local_space_from_space(isl_set_get_space(func_set));
	ISL::Constraint c;
	for(size_t i=0; i<size(); ++i){
		c = isl_constraint_alloc_equality(ISL::copy(ls));
		c = isl_constraint_set_coefficient_si(c, isl_dim_set, i, 1);
		c = isl_constraint_set_coefficient_si(c, isl_dim_param, i, -1);
		func_set = isl_set_add_constraint(func_set, c);
	}
	return func_set;
}

ISL::Space List_t::create_dimension_space() const {
	ISL::Space r = isl_space_set_alloc(ctx, size(), size());
	for(size_t i=0; i<size(); ++i)
		r = isl_space_set_dim_id(r, isl_dim_param, i, isl_id_alloc(ctx, l[i].c_str(), nullptr));
	return r;
}

//Add params in l if not present
List_t List_t::merge(const List_t& params){
	for(auto& param : params)
		if(!present(param))
			l.insert(l.begin(), param);
	return *this;
}

bool List_t::present(const string& s) const{
	for(const auto& p : l)
		if(s == p)
			return true;
	return false;
}

std::string& List_t::operator[](int i){
	return l[i];
}

int List_t::pos(string s) const{
	for(size_t i=0; i<size(); i++)
		if(l[i] == s)
			return i;
	return -1;
}

int List_t::include(List_t* param){
	for(size_t i=0; i<param->size(); i++)
		if(!present(param->get(i)))
			return i;
	return -1;
}

void List_t::name_map(ISL::Map& map, isl_dim_type type_isl, uint offset) const{
	if(isl_map_dim(map, type_isl) - offset < size()){
		std::cerr << "*** Error: name_map was impossible (offset=" << offset << ")\n" << std::endl;
		ISL::print(map);
		this->print();
		abort();
	}else{
		for(uint i=0; i<size(); ++i)
			map = isl_map_set_dim_id(map, type_isl, i+offset, isl_id_alloc(ctx, l[i].c_str(), nullptr));
	}
}

void List_t::name_set(ISL::Set& set, isl_dim_type type_isl, uint offset) const{
	if(isl_set_dim(set, type_isl) - offset < size()){
		std::cerr << "*** Error: name_set was impossible (offset=" << offset << ")\n" << std::endl;
		ISL::print(set);
		this->print();
		abort();
	}else{
		for(uint i=0; i<size(); ++i)
			set = isl_set_set_dim_id(set, type_isl, i+offset, isl_id_alloc(ctx, l[i].c_str(), nullptr));
	}
}

ISL::L_Id List_t::to_id_list() const{
	ISL::L_Id r = isl_id_list_alloc(ctx, size());
	for(const auto& s : l)
		r = isl_id_list_add(r, isl_id_alloc(ctx, s.c_str(), NULL));
	return r;
}

void List_t::print() const{
	for(size_t i=0; i<l.size(); i++)
		std::cout << l[i] << add_str_if(i, l.size(), " | ");
	std::cout << std::endl;
}