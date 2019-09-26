#ifndef OMP_PARALLEL_REGION_T_HPP
#define OMP_PARALLEL_REGION_T_HPP

#include <string>
#include <Codes/Code_t.hpp>

class List_t;

class OMP_Parallel_Region_t : public Code_t{
	
	List_t* privates;

	std::string inner_code;
	
public:

	OMP_Parallel_Region_t(List_t* _privates, const std::string& _inner);
		
	virtual std::string to_cpp_string() const;

};

#endif // MULT_ARRAY_T_HPP
