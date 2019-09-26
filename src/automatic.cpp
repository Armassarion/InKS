#include <iostream>
#include <unistd.h>
#include <fstream>
#include <ctime>

#include <Codes/Exit_t.hpp>
#include <Codes/If_t.hpp>
#include <Implem_t.hpp>

#include <isl_processing.hpp>
#include <Function_Map.hpp>
#include <Generate_t.hpp>
#include <automatic.hpp>
#include <parser.tab.hh>
#include <isl_utils.hpp>
#include <utils.hpp>

using std::ofstream;
using std::ifstream;
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::ios;

void Auto::parsing(string path){
	clock_t st = clock();
	ifstream input_file(path, ios::in);
	if(input_file.is_open()){
		string to_parse((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
		YY_BUFFER_STATE auto_buff = auto_scan_string(to_parse.c_str());
		autoparse();
		auto_delete_buffer(auto_buff);
	}else{
		cerr << "*** Error: Unable to open " << path << endl;
		exit(EXIT_FAILURE);
	}	
	print_elapsed_time("* Semantics parsing: ", st);
}

void Auto::choices_generation(string gen_out, Function_Map& Functions, Generate_t& Gen){
	Implem_t implem(Gen.name, Gen.Vin);
	ISL::UMap Sch_map;
	
	{
		std::vector<Array*> Sorted_Arrays(Gen.Arrays->size());
		for(const auto& pair_array : *Gen.Arrays)
			Sorted_Arrays[pair_array.second->array_id] = pair_array.second;
			
		for(Array* A : Sorted_Arrays)
			if(A->is_IO())
				implem.push_io(IO_t(A->name, A->type, A->dim_size));
	}
	
	Generate_InOut_copy(implem);
	implem.push_code(new If_t(new Exit_t("Invalid parametric values. Exiting..."), valid_parameters, true));
	
	clock_t st = clock();
	std::string Scheduling = parse_ast_node(Generate_Scheduling(Sch_map));
	print_elapsed_time("* Scheduling: ", st);
	
	st = clock();
	Generate_Mapping(implem, Sch_map);
	print_elapsed_time("* Memory mapping: ", st);
	
	implem.push_code(Scheduling);
	
	ofstream hppfile(gen_out);
	{
		string include_guard = generate_include_guard(gen_out);
		hppfile << "#ifndef " << include_guard << endl;
		hppfile << "#define " << include_guard << endl;
		hppfile << "#include <limits>"<< endl;
		hppfile << "#include <InKSArray.hpp>" << endl << endl;
		hppfile << get_isl_macro() << endl << endl;
		
		hppfile << implem.to_auto_string();
		
		hppfile << endl << "#endif";
	}
	hppfile.close();
	
	execl("/usr/bin/astyle", "/usr/bin/astyle", gen_out.c_str(), NULL);
}
