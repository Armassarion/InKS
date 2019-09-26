#ifndef UTILS__
#define UTILS__

#include <boost/program_options.hpp>
#include <iostream>
#include <vector>
#include <string>

void init_global();

std::string& replace_all(std::string&, std::vector<std::string>&, std::vector<std::string>&);
std::string& replace_all(std::string&, std::vector<std::string>&, std::string="");
std::string& replace_all(std::string&, std::string, std::string="");

std::vector<std::string> split(std::string&, std::string, const int=0);

std::string& delete_char(std::string& in, std::string search);
std::string& delete_last(std::string &, int=1);
std::string delete_last(std::string &&, int=1);

bool is_a_number(const std::string&);
std::string& trim(std::string&);
std::string cptrim(std::string);

void print_elapsed_time(std::string, clock_t);
double elapsed_time(clock_t);

std::string default_path(boost::program_options::variables_map&, std::string, std::string, std::string="");
std::string generate_include_guard(const std::string& filename);
std::string generate_namespace(const std::string& filename);
std::string get_filename(std::string path);
std::string add_coma_if(int, int);
std::string add_str_if(int, int, const std::string&);

template<typename T>
void printV(const T& v){
	for(const auto& val : v) std::cout << "|" << val << "|\t";
	std::cout << std::endl;
}

#endif