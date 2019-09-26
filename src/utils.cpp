#include <algorithm>
#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

#include <isl/schedule.h>
#include <isl/options.h>
#include <isl/ast.h>
#include <isl/ctx.h>

#include <const_globals.hpp>
#include <utils.hpp>

using vstring = std::vector<std::string>;
using oss = std::ostringstream;
using std::ifstream;
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::ios;

void init_global(){
	ctx = isl_ctx_alloc();
	//isl_options_set_schedule_maximize_coincidence(ctx, 1);
	isl_options_set_schedule_treat_coalescing(ctx, 1);
	isl_options_set_ast_always_print_block(ctx, 1);
	isl_options_set_on_error(ctx, ISL_ON_ERROR_ABORT);
}

bool is_a_number(const string& s){
	if(s.front() == '-' && s.length() > 1)
		return s.find_first_not_of("0123456789", 1) == string::npos;
	
	return s.find_first_not_of("0123456789") == string::npos;
}

string& trim(string& in){
	unsigned int i;
	int space_l = 0;
	for(i=0; i<in.size() && isspace(in[i]); i++) ++space_l;
	in.erase(0, space_l);

	for(i=in.size(); i>0 && isspace(in[i-1]); i--);
	if(i <= in.size()) in.erase(i, string::npos);
	
	return in;
}

string cptrim(string in){
	return trim(in);
}

vstring split(string& s, string delim, const int max){
	vstring r;
	trim(s);
	auto start = 0U;
	auto end = s.find(delim);
	string item;
	int nb = 0;
	while ( ((max != 0 && nb < max) || max == 0) && end != string::npos){
		item = s.substr(start, end - start);
		r.push_back(trim(item));
		start = end + delim.length();
		end = s.find(delim, start);
		nb++;
	}
	item = s.substr(start, string::npos);
	r.push_back(trim(item));
	return r;
}

string& delete_last(string& in, int size){
	return in.erase(in.size()-size);
}

string delete_last(string&& in, int size){
	return in.erase(in.size()-size);
}

string& delete_char(string& in, string search){
	size_t r;
	while( (r=in.find(search)) != string::npos)
		in.erase(r, 1);
	return in;
}

string get_filename(string path){
	size_t lastSlash = path.find_last_of("/");
	size_t lastPoint = path.find_last_of(".");
	return path.substr(lastSlash+1, lastPoint-lastSlash-1);
}

string& replace_all(string& s, string pattern, string replacer){
	size_t pos;
	while( (pos = s.find(pattern)) != string::npos)
		s.replace(pos, pattern.length(), replacer);
	return s;
}

string& replace_all(string& s, vstring& patterns, string replacer){
	for(string pattern : patterns) replace_all(s, pattern, replacer);
	return s;
}

string& replace_all(string& s, vstring& patterns, vstring& replacers){
	for(size_t i = 0; i<patterns.size(); i++)
		replace_all(s, patterns[i], replacers[i]);
	return s;
}

double elapsed_time(clock_t s){
	return ((double)clock()-(double)s)/(double)CLOCKS_PER_SEC;
}

void print_elapsed_time(std::string s, clock_t st){
	double time = elapsed_time(st);
	cerr << s << " " << std::fixed << std::setprecision(2) << time << "s" << std::endl;
}

std::string default_path(boost::program_options::variables_map& vm, std::string key, std::string default_path, std::string extension){
	if(vm.count(key))		return vm[key].as<string>();
	if(extension.empty())	extension = ".hpp";
	
	return default_path + extension;
}

std::string generate_include_guard(const std::string& filename){
	string include_guard("INKS_GENERATED_");
	for (char fcar: filename){
		if(isalnum(fcar))	include_guard += toupper(fcar);
		else				include_guard += '_';
	}
	return include_guard + "__";
}

std::string generate_namespace(const std::string& filename){
	string namespace_name;
	for (char fcar: filename){
		if(isalnum(fcar))	namespace_name += toupper(fcar);
		else				namespace_name += '_';
	}
	return namespace_name;
}

std::string add_coma_if(int i, int cpt){
	return add_str_if(i, cpt, ", ");
}

std::string add_str_if(int i, int cpt, const std::string& str){
	return i < cpt - 1 ? str : "";
}