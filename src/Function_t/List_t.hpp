#ifndef LIST_T_HPP_
#define LIST_T_HPP_

#include <isl_cpp_type.hpp>
#include <vector>
#include <string>

class V_Input_t;

struct List_t{
	std::vector<std::string> l;

	enum Add_param {T, F};

	List_t();

	List_t(const List_t&);
	
	List_t(char* s);
	
	List_t(ISL::Set s, Add_param=F);
	
	List_t(ISL::BSet s, Add_param=F);
	
	List_t(ISL::Map s, Add_param=F);
	
	List_t(ISL::BMap s, Add_param=F);
	
	List_t(std::string s);

	List_t* push(char* s);
	
	List_t* push(std::string s);

	size_t size() const;

	const char* get(int i) const;

	std::vector<std::string>::iterator begin();

	std::vector<std::string>::iterator end();

	const std::vector<std::string>::const_iterator begin() const ;

	const std::vector<std::string>::const_iterator end() const ;

	std::string fill_param() const;
	
	ISL::Set& name_isl_set(ISL::Set&) const ;
	
	ISL::Set create_dimension_set(const V_Input_t&) const ;
	
	ISL::Set create_dimension_set(ISL::Space spc) const ;

	ISL::Space create_dimension_space() const ;
		
	List_t merge(const List_t& params); /*Ajoutes les param de params dans l s'ils n'y sont pas*/
	
	bool present(const std::string& s) const;
		
	std::string& operator[](int);
	
	int pos(std::string) const;
	
	int include(List_t*); /*check that "this" contains all the dim of param, if not, return the first dim that is not included*/
	
	void name_map(ISL::Map& map, isl_dim_type, uint offset=0) const;
		
	void name_set(ISL::Set& set, isl_dim_type, uint offset=0) const;
	
	ISL::L_Id to_id_list() const;
	
	void print() const;
};


#endif /* LIST_T_HPP_ */
