#ifndef DD_MEM_T_HPP
#define DD_MEM_T_HPP

#include <string>
#include <map>

#include <isl_cpp_type.hpp>
#include <DD_Line_t.hpp>
#include <Point_t.hpp>

class DD_Scope_t;
class DD_Gen_t;
class Set_Poly_t;

class DD_Mem_t : public DD_Line_t{

public:
	enum OWN {Give, Keep, Take};

protected:

	std::string type;

	std::string physical_id;
	
	Set_Poly_t* sizes;
	
	OWN own_prop;
	
	ISL::Set sizes_isl;
	
	//In case of an allocation with a range constraint, it is necessary to add one to the size of each dim
	bool plus_one;
	
public:
	
	template <typename T>
	DD_Mem_t(int _lineno, T _type, T _pid, Set_Poly_t* _sizes = new Point_t(), OWN _prop = OWN::Take, bool _po=false) : 
		DD_Line_t(_lineno),
		type(_type),
		physical_id(_pid),
		sizes(_sizes),
		own_prop(_prop),
		sizes_isl(nullptr),
		plus_one(_po)
	{}

	DD_Mem_t(const DD_Mem_t& src);
	
	virtual void generate(Implem_t& implem, const Time_Event_t& events, ISL::Set context);
	
	virtual void check(const Time_Event_t& events);
	
	virtual void scope(DD_Scope_t& scope_object);
	
	std::string get_param();
	
	ISL::PWAff dim_size(uint i);
	
	DD_Mem_t& operator=(DD_Mem_t& _m);
	
	std::string get_physical_id() const;
		
	std::string get_type() const;
	
	size_t get_dim_size() const;
	
	std::string get_sizes() const;
	
	std::string get_mult() const;
	
	void print() const;
	
	bool is_give() const;

	bool is_keep() const;

	bool is_take() const;

};

#endif // DD_MEM_T_HPP
