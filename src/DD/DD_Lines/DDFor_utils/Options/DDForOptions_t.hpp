#ifndef DDFOROPTIONS_T_HPP
#define DDFOROPTIONS_T_HPP

#include <vector>
#include <string>

class DDForOption_t;

class DDForOptions_t{
	
	std::vector<DDForOption_t*> options;
	
public:

	DDForOptions_t(){};
	
	DDForOptions_t(DDForOption_t*);
	
	DDForOptions_t* push(DDForOption_t*);
	
	std::string add_str_before();

};

#endif // DDFOROPTIONS_T_HPP
