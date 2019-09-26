#include <iostream>
#include <fstream>
#include <string>

#include <const_globals.hpp>
#include <Function_Map.hpp>
#include <Include_t.hpp>
#include <utils.hpp>

using std::ofstream;
using std::string;
using std::endl;

Include_t::Include_t() : 
	include("")
{}

void Include_t::push(const char* _input)
{
	std::string inc(_input);
	inc.erase(inc.find("#END"));
	size_t first_line_end = inc.find("\n");
	inc.erase(0, first_line_end); /*#Header (c)*/
	include += inc;
}

std::string Include_t::to_string() const{
	return include;
}
