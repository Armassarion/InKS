#ifndef OMP_FOR_OPTION_T_HPP
#define OMP_FOR_OPTION_T_HPP

#include <DDFor_utils/Options/DDForOption_t.hpp>

class List_t;

class OMP_For_option_t : public DDForOption_t{
	
	List_t* privates;
	
	const char* collapse;
	
	bool parallel;
	
public:

	OMP_For_option_t(const char* _collapse, List_t* _privates, bool _parallel=false);
	
	virtual std::string add_str_before();
};

#endif // OMP_FOR_OPTION_T_HPP
