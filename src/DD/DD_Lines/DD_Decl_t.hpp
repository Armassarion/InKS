#ifndef DD_DECL_HPP
#define DD_DECL_HPP

#include <iostream>
#include <string>

#include <DD_Line_t.hpp>

class DD_Gen_t;

class DD_Decl_t : public DD_Line_t{

private :

	std::string type;
	
	std::string id;
	
	int dim;
	
public:
	
	template <typename T>
	DD_Decl_t(int _lineno, T _type, T _id, int _dim = 0) : 
		DD_Line_t(_lineno),
		type(_type),
		id(_id),
		dim(_dim)
	{}
	
	DD_Decl_t(const DD_Decl_t& src);
	
	void generate(Implem_t&, const Time_Event_t& events, ISL::Set context);
	
	void check(const Time_Event_t& events);
	
	void scope(DD_Scope_t& scope_object);
	
	bool match(const std::string& src_type, int src_dim) const;
	
	int get_dim() const;
	
	void print(){
		std::cout << type << " " << id << "(" << dim << ")" << std::endl;
	}
	
	std::string get_physical_id() const;

};

#endif // DD_DECL_HPP
