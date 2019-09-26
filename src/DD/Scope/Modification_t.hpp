#ifndef MODIFICATION_T_HPP
#define MODIFICATION_T_HPP

#include <const_globals.hpp>
#include <isl_cpp_type.hpp>
#include <isl_utils.hpp>
#include <map>

class DD_Line_t;

struct Time_type_t{
	int val;
	
	Time_type_t() : 
		val(1)
	{}
	
	Time_type_t operator=(const Time_type_t& t){
		val = t.val;
	}
	
	Time_type_t operator++(int){
		val++;
		return *this;
	}
	
	void set_dim_to_time(ISL::Set& sch, int dim){
		//set_add_value_to_dim(sch, val, dim);
		set_set_dim_to_const(sch, val, dim);
	}
};

class Modification_t{
	
protected:

	DD_Line_t* line;
	
	ISL::Set mysch;
	
public:

	using time_type_t = Time_type_t;
	
	static time_type_t init_time(){
		return Time_type_t();
	}

	enum Type {Map, Unmap, Read, Write, ReadPhy, WritePhy, AddFor, DelFor};
	
	using Time_to_action_t = std::map<Type, ISL::UMap>;
	
	Modification_t(){};
	
	Modification_t(DD_Line_t* _line);
			
	int get_lineno() const;
	
	virtual int sch_size() const;
	
	ISL::UMap& sch_to_modif_base(ISL::Set& sch, time_type_t& time, int& dim, Time_to_action_t& tta);
	
	virtual void sch_to_modif(ISL::Set& sch, time_type_t& time, int& dim, Time_to_action_t& t) = 0;
		
	virtual Type modif_type() const = 0;
	
	virtual void print() const = 0;
};

class AddMap_t : public Modification_t{

	ISL::Map m;
	
public:
	
	AddMap_t(DD_Line_t* _line, ISL::Map _m);
		
	void sch_to_modif(ISL::Set& sch, time_type_t& time, int& dim, Time_to_action_t&);
	
	Type modif_type() const;
	
	void print() const;
};

class AddUnmap_t : public Modification_t{

	ISL::Set unmap;
	
public:
	
	AddUnmap_t(DD_Line_t* _line, ISL::Set _u);
	
	void sch_to_modif(ISL::Set& sch, time_type_t& time, int& dim, Time_to_action_t&);
	
	Type modif_type() const;
	
	void print() const;
};

class AddRead_t : public Modification_t{

protected:
	
	ISL::USet set;
	
public:
	
	AddRead_t(DD_Line_t* _line, ISL::USet _set);
		
	void sch_to_modif(ISL::Set& sch, time_type_t& time, int& dim, Time_to_action_t&);
	
	virtual Type modif_type() const;
	
	virtual const char* modif_print() const;
	
	virtual void print() const;
};

class AddWrite_t : public AddRead_t{
	
public:
	
	AddWrite_t(DD_Line_t* _line, ISL::USet _set);
	
	Type modif_type() const;
	
	virtual const char* modif_print() const;
};

class AddReadPhy_t : public AddRead_t{

public:
	
	AddReadPhy_t(DD_Line_t* _line, ISL::USet _set);
		
	virtual Type modif_type() const;
	
	virtual const char* modif_print() const;
	
};

class AddWritePhy_t : public AddRead_t{
	
public:
	
	AddWritePhy_t(DD_Line_t* _line, ISL::USet _set);
	
	Type modif_type() const;
	
	virtual const char* modif_print() const;
};


class AddFor_t : public Modification_t{

	ISL::Set vars;
	
	int nb_action;
	
public:
	
	Time_type_t original_time;
	
	AddFor_t(DD_Line_t* _line, ISL::Set s, int _nb_action);
	
	int sch_size() const;
	
	void sch_to_modif(ISL::Set& sch, time_type_t& time, int& dim, Time_to_action_t&);
	
	Type modif_type() const;
	
	void print() const;
};

class DelFor_t : public Modification_t{

	ISL::Set vars;
	
	Time_type_t& original_time;
	
public:
	
	DelFor_t(DD_Line_t* _line, ISL::Set s, Time_type_t& t);
	
	int sch_size() const;
	
	void sch_to_modif(ISL::Set& sch, time_type_t& time, int& dim, Time_to_action_t&);
		
	Type modif_type() const;
	
	void print() const;
};

#endif // MODIFICATION_T_HPP
