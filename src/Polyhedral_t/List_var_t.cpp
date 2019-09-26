#include <Inequality_t.hpp>
#include <List_var_t.hpp>
#include <List_t.hpp>
#include <Var_t.hpp>

List_var_t::List_var_t(Var_t* _v){
	ranges.push_back(_v);
}

List_var_t* List_var_t::push(Var_t* _v){
	ranges.push_back(_v);
	return this;
}

Var_t* List_var_t::get_var_poly(){
	if(this->size() == 0) return new Var_t("", new Inequality_t(nullptr, nullptr));
	
	for(size_t i=1; i<ranges.size(); i++)
		ranges[i-1]->set_inner(ranges[i]);
	return ranges[0];
}

Set_Poly_t* List_var_t::get_poly(Set_Poly_t* s){
	if(this->size() == 0) return s;
	
	for(size_t i=1; i<ranges.size(); i++)
		ranges[i-1]->set_inner(ranges[i]);

	ranges.back()->set_inner(s);
	return ranges[0];
}

List_t List_var_t::get_var_names() const{
	List_t l;
	for(const Var_t* f : ranges)
		l.push(f->get_name());
	return l;
}

size_t List_var_t::size() const{
	return ranges.size();
}

Var_t* List_var_t::get(int i){
	return ranges[i]->clone();
}

std::string List_var_t::get_var_name(int i) const{
	return std::string(ranges[i]->get_name());
}

void List_var_t::merge(List_var_t& _vars){
	for(Var_t* v : _vars)
		ranges.emplace_back(std::move(v));
}

void List_var_t::print() const{
	for(Var_t* v : ranges)
		std::cout << v->get_name() << std::endl;
}

std::vector<Var_t*>::iterator List_var_t::begin(){
	return ranges.begin();
}

std::vector<Var_t*>::iterator List_var_t::end(){
	return ranges.end();
}