#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <string>

namespace po = boost::program_options;

using std::exception;
using std::string;
using std::cerr;
using std::cout;
using std::endl;

#include <isl/version.h>

#include <isl_processing.hpp>
#include <const_globals.hpp>
#include <V_DD_Block_t.hpp>
#include <globals.hpp>
#include <utils.hpp>

#include <automatic.hpp>
#include <DD.hpp>

Function_Map Functions;
Include_t Includes;
Generate_t Gen;

bool inline_mode;
bool debug_mode;
bool verbosity;
isl_ctx* ctx;

#include <Modulo_t.hpp>
#include <Nombre_t.hpp>
#include <Reference_t.hpp>
#include <Inequality_t.hpp>
#include <Equality_t.hpp>
#include <isl_cpp_type.hpp>
#include <isl_utils.hpp>
#include <Point_t.hpp>
#include <Poly_scope_t.hpp>
#include <Input_t.hpp>
#include <List_t.hpp>
#include <Union_Poly_t.hpp>
#include <Var_t.hpp>

int main(int argc, char** argv){
	init_global();
	
	{
		Constraint_t* m = new Modulo_t(new Equality_t(new Reference_t("t")), "3");
		Constraint_t* m2 = new Equality_t(new Reference_t("t"));
		Point_t* p = new Point_t(m2);
		
		Union_Poly_t* up = new Union_Poly_t(new For_t("t", new Inequality_t(new Nombre_t("0"), new Nombre_t("10")), p));
		
		V_Input_t vin;
		ISL::Set func_set = isl_set_read_from_str(ctx, "{[s]}");
		Poly_scope_t polyscope(vin, ISL::copy(func_set));
		up->scope(polyscope);
		ISL::print(up->createMap(vin, func_set));
	}
	
	abort();
	
	po::options_description visible("Usage: inks_p [options] <file>\nOptions");
	visible.add_options()
		("help,h",			"Display this short help and exit.")
		("verbose,v",		"Increases verbosity.")
		("version,V",		"Display the version number and exit.")
		("inline,i", 		"Add inline directives to kernel calls")
		("debug,d", 		"Generate code with debugging features")
		("output,o",		po::value<string>()->value_name("file"), "Place the output (computational kernel) into file")
		("auto,a",			po::value<string>()->value_name("file"), "Generates an automatic execution choice into file.")
		("inkspso,e",		po::value<string>()->value_name("file"), "Path of the DD file for DD generation.")
		("out-inksodd,f",	po::value<string>()->value_name("file"), "Generates an DD execution choice into file.")
	;
	
	po::options_description all_options("All options");
	all_options.add_options()
		("input",      po::value<string>()->required()->value_name("file"), "Path of the InKS file.")
	;
// 	visible.add(all_options);
	all_options.add(visible);
	
	po::positional_options_description p;
	p.add("input", 1);

	po::variables_map vm;
	try {
		po::store(po::command_line_parser(argc, argv).options(all_options).positional(p).run(), vm);
		po::notify(vm);
	} catch (const exception& e) {
		cerr << "*** Error: " << e.what() << endl << endl;
		cerr << visible;
		return EXIT_FAILURE;
	}
	
	if(vm.count("help")){
		cout << visible << endl;
		return EXIT_SUCCESS;
	}
	
	if(vm.count("version")){
		cerr << "ISL VERSION: " << isl_version();
		cerr << "InKS VERSION: pre-release 3" << endl;
		return EXIT_SUCCESS;
	}
	
	verbosity 	= (bool)vm.count("verbose");
	inline_mode	= (bool)vm.count("inline");
	debug_mode	= (bool)vm.count("debug");
	
	string semantics_path = vm["input"].as<string>();
	string semantics_filename = get_filename(semantics_path);
	string auto_out, hpp_out, dd_out, loop_out, xmp_out;
		
	hpp_out  = default_path(vm, "output", semantics_filename + ".kernels");
	dd_out 	 = default_path(vm, "out-inksodd", semantics_filename + ".inkso.dd");
	auto_out = default_path(vm, "auto", semantics_filename + ".generic_choices");
	
	cerr << "* InKSpia file: " << semantics_filename << endl;
	cerr << "* Output kernel file: " << hpp_out << endl;
	
	Auto::parsing(semantics_path);
	Functions.gen_hpp_file(Includes, hpp_out);
	
	if(vm.count("inkspso") || vm.count("auto")){
		Gen.scope(Functions);
		Initialize_ISL_Object();
	}

	if(vm.count("inkspso")){
		cerr << "* InKSoDD generation in " << dd_out << std::endl;
		DD::parse_and_generate(vm["inkspso"].as<string>(), dd_out, hpp_out, Functions, Gen);
	}
	
/*Automatic version generation*/
	if(vm.count("auto")){
		cerr << "* Generic choices generation in " << auto_out << std::endl;
		Auto::choices_generation(auto_out, Functions, Gen);
	}

	cerr << "* Compilation succeed!" << endl;
	return EXIT_SUCCESS;
}
