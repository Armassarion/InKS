#include <const_globals.hpp>
#include <isl_utils.hpp>
#include <Input_t.hpp>
#include <List_t.hpp>
#include <utils.hpp>
#include <sstream>

using std::string;

Input_t::Input_t(std::string _name) : 
	name(_name)
{
	id = isl_id_alloc(ctx, _name.c_str(), nullptr);
}

Input_t::Input_t(const char* _name) : 
	Input_t(std::string(_name))
{}

Input_t::Input_t(Input_t&& i) : 
	name(std::move(i.name)),
	id(std::move(i.id))
{
	i.name = "";
	i.id = nullptr;
}

Input_t::Input_t(const Input_t& i) : 
	name(i.name)
{
	id = ISL::copy(i.id);
}

Input_t::Input_t() : 
	name(""),
	id(nullptr)
{}

Input_t& Input_t::operator=(const Input_t& i){
	id = ISL::copy(i.id);
	name = i.name;
	return *this;
}

Input_t::~Input_t(){
	isl_id_free(id);
}
	
V_Input_t::V_Input_t()
{}

V_Input_t::V_Input_t(const V_Input_t& v) : 
	vin(v.begin(), v.end())
{}

V_Input_t& V_Input_t::merge(const V_Input_t& v){
	vin.insert(vin.end(), v.begin(), v.end());
	return *this;
}

V_Input_t& V_Input_t::merge(const List_t* l){
	vin.insert(vin.end(), l->begin(), l->end());
	return *this;
}
	
V_Input_t::V_Input_t(const List_t* l) : 
	vin(l->begin(), l->end())
{}

V_Input_t& V_Input_t::push(const char* _name){
	vin.emplace_back(_name);
	return *this;
}

void V_Input_t::init(const List_t* l){
	vin = std::vector<Input_t>(l->begin(), l->end());
}

V_Input_t& V_Input_t::operator=(const V_Input_t& v){
	vin = std::vector<Input_t>(v.begin(), v.end());
	return *this;
}

bool V_Input_t::operator==(const V_Input_t& v) const {
	if(v.size() != this->size())
		return false;
		
	for(size_t i=0; i<this->size(); i++)
		if(this->get_name(i) != v.get_name(i))
			return false;
	
	return true;
}

Input_t& V_Input_t::operator[](int i){
	return vin[i];
}

const Input_t& V_Input_t::operator[](int i) const{
	return vin[i];
}

std::string V_Input_t::get_name(size_t i) const{
	return vin[i].name;
}

size_t V_Input_t::size() const{
	return vin.size();
}

const Input_t& V_Input_t::struct_input(size_t i) const {
	return vin[i];
}

ISL::Id V_Input_t::get_isl_id(size_t i) const {
	return ISL::copy(vin[i].id);
}

int V_Input_t::get_constant_pos(const char* _name) const{
	std::string compare(_name);
	for(size_t i=0; i<vin.size(); ++i)
		if(vin[i].name == compare)
			return i;
	return -1;
}

ISL::Space& V_Input_t::fill_input_id(ISL::Space& spc) const {
	for(size_t i=0; i<size(); ++i)
		spc = isl_space_set_dim_id(spc, isl_dim_param, i, get_isl_id(i));
	return spc;
}

ISL::Space V_Input_t::map_space(int dim_in, int dim_out) const{
	ISL::Space spc = isl_space_alloc(ctx, size(), dim_in, dim_out);
	return fill_input_id(spc);
}

ISL::Space V_Input_t::set_space(int dim) const{
	ISL::Space spc = isl_space_set_alloc(ctx, size(), dim);
	return fill_input_id(spc);
}

ISL::Space V_Input_t::param_space() const{
	ISL::Space spc = isl_space_params_alloc(ctx, size());
	return fill_input_id(spc);
}

std::string V_Input_t::get_params(bool type) const{
	std::ostringstream params;
	for(uint i=0; i<vin.size(); ++i)
		params << (type ? "const int " : "") << vin[i].name << add_coma_if(i, vin.size());
	return params.str();
}

std::vector<Input_t>::iterator V_Input_t::begin(){
	return vin.begin();
}

std::vector<Input_t>::iterator V_Input_t::end(){
	return vin.end();
}

const std::vector<Input_t>::const_iterator V_Input_t::begin() const{
	return vin.begin();
}

const std::vector<Input_t>::const_iterator V_Input_t::end() const{
	return vin.end();
}

void V_Input_t::print() const{
	for(uint i=0; i<vin.size(); ++i)
		std::cout << vin[i].name << add_str_if(i, vin.size(), " | ");
	std::cout << std::endl;
}