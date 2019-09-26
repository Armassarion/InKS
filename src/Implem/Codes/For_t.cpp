#include <sstream>

#include <Codes/For_t.hpp>
#include <isl_utils.hpp>

For_t::For_t(const std::string& _iterator, ISL::Ast_expr _min, ISL::Ast_expr _cond, ISL::Ast_expr _inc) : 
	iterator_name(_iterator),
	min(_min),
	cond(_cond),
	inc(_inc),
	inner(nullptr),
	before_outside(nullptr),
	before_inside(nullptr),
	after_outside(nullptr),
	after_inside(nullptr)
{}

void For_t::set_inner(Code_t* c){
	inner = c;
}

void For_t::set_before_outside(Code_t* c){
	before_outside = c;
}

void For_t::set_before_inside(Code_t* c){
	before_inside = c;
}

void For_t::set_after_outside(Code_t* c){
	after_outside = c;
}

void For_t::set_after_inside(Code_t* c){
	after_inside = c;
}

std::string For_t::to_cpp_string() const{
	std::ostringstream r;
	
	if(before_outside != nullptr) r << before_outside->to_cpp_string() << "\n";
	{
		//min
		r << "for(int " << iterator_name << " = " << print_C_ast_expr(min) << "; ";
		//cond
		r << print_C_ast_expr(cond) << "; ";
		//increment
		r << iterator_name << " += " << print_C_ast_expr(inc) << ")";
	}
	
	r << "{\n";
		if(before_inside != nullptr) r << before_inside->to_cpp_string() << "\n";
		
		if(inner != nullptr) r << inner->to_cpp_string() << "\n";
		else std::cout << iterator_name <<  " " << inner << std::endl;
			
		if(after_inside != nullptr) r << after_inside->to_cpp_string() << "\n";
	r << "}\n";
	
	if(after_outside != nullptr) r << after_outside->to_cpp_string() << "\n";
	
	return r.str();
}