#include <unistd.h>
#include <iostream>
#include <fstream>
#include <ctime>

#include <isl_processing.hpp>
#include <dd_parser.tab.hh>
#include <Function_Map.hpp>
#include <Generate_t.hpp>
#include <isl_utils.hpp>
#include <Implem_t.hpp>
#include <Input_t.hpp>
#include <utils.hpp>
#include <DD.hpp>

using std::ifstream;
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::ios;

void DD::parsing(string path, V_DD_Block_t& DD_Blocks, Generate_t& Gen){
	clock_t st = clock();
	ifstream input_file(path, ios::in);
	if(input_file.is_open()){
		string to_parse((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
		YY_BUFFER_STATE DD_buff = DD_scan_string(to_parse.c_str());
		DDparse(&DD_Blocks);
		DD_delete_buffer(DD_buff);
	}else{
		cerr << "*** Error: Unable to open " << path << endl;
		exit(EXIT_FAILURE);
	}
	print_elapsed_time("* InKSPSO parsing: ", st);
	
	st = clock();
	DD_Blocks.resolve();
	DD_Blocks.scope(valid_parameters, Gen);
	
	print_elapsed_time("* Total InKSPSO scope: ", st);
}

std::string DD::choices_generation(V_DD_Block_t& DD_Blocks, Function_Map& Functions, Generate_t& Gen){
	std::ostringstream r;
		
	clock_t st = clock();
	std::vector<Implem_t> implems(std::move(DD_Blocks.generate()));
	for(auto& implem : implems)
		r << implem.to_cpp_string() << std::endl << std::endl;
		
	print_elapsed_time("* InKSPSO generation: ", st);
	
	if(verbosity) std::cout << r.str() << std::endl;
	return r.str();
}

void DD::parse_and_generate(std::string in, std::string out, std::string kernel_file, Function_Map& Functions, Generate_t& Gen){
	V_DD_Block_t DD_Blocks;
	
	//try{
		parsing(in, DD_Blocks, Gen);
		
		std::ofstream hppfile(out);
		{
			string include_guard = generate_include_guard(out);
			hppfile << "#ifndef " << include_guard << endl;
			hppfile << "#define " << include_guard << endl;
			hppfile << "#include <array>" << endl;
			hppfile << "#include \"" << kernel_file << "\"" << endl << endl;
			
			if(add_namespace) hppfile << "namespace " << generate_namespace(out) << "{" << endl;
			hppfile << get_isl_macro() << endl << endl;
			hppfile << choices_generation(DD_Blocks, Functions, Gen);
			if(add_namespace) hppfile << "}\n";
			
			hppfile << endl << "#endif";
		}
		hppfile.close();
		/*
	}catch(const std::exception& e){
		std::cerr << "[InKSoDD exception] " << e.what() << std::endl;
		abort();
	}
	*/
	execl("/usr/bin/astyle", "/usr/bin/astyle", out.c_str(), NULL);
}