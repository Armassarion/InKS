#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "array2d.hpp"
#include "heat_equation.kernels.hpp"

#ifdef USE_AUTO
	#include "heat_equation.sched.hpp"
	#define DUMP_FILE "auto.dat"
#elif USE_AUTO_INF
	#include "heat_equation.sched2.hpp"
	#define DUMP_FILE "auto_inf.dat"
#elif USE_INKSO1
	#include "heat_equation1.inkso.hpp"
	#define DUMP_FILE "inkso1.dat"
#elif USE_INKSO2
	#include "heat_equation2.inkso.hpp"
	#define DUMP_FILE "inkso2.dat"
#else
	#include "sched.hpp"
	#define DUMP_FILE "opt.dat"
#endif

using std::cerr;
using std::cout;
using std::endl;
using std::istringstream;

int main ( int argc, char* argv[] )
{
	if ( argc != 3 ) {
		cerr << "Usage: "<<argv[0]<<" <sizex> <nb_iter>"<<endl;
		return EXIT_FAILURE;
	}
	
	istringstream sizex_str(argv[1]);
	size_t sizex;
	sizex_str >> sizex;

	istringstream sizet_str(argv[2]);
	size_t sizet;
	sizet_str >> sizet;
	
	cout << "Problem: x="<<sizex<<" x t="<<sizet<<endl;
	
	Array2D heat(sizex);
	double* heat_ptr;
	clock_t st = clock();
#if defined(USE_INKSO1) || defined(USE_INKSO2)
		inks_heat(heat_ptr, sizex, sizet);
#else
		inks_heat(heat, sizex, sizet);
#endif
	std::cout << (((double)clock()-st)/CLOCKS_PER_SEC) << std::endl;

	cout << "Result: { ";
	for ( int x=0; x<sizex; ++x ) {
#if defined(USE_INKSO1) || defined(USE_INKSO2)
		cout << heat_ptr[x + ((sizet-1)%2) * sizex];
#else
		cout << heat(x, sizet-1);
#endif
		if ( (x+1)<sizex ) cout << ", ";
	}
	cout << " }"<<endl;

	int fd = open(DUMP_FILE, O_CREAT|O_WRONLY, S_IRWXU|S_IRWXG|S_IRWXO);
	if(fd != -1){
		for(int i=0; i<sizex; i++){
			if(write(fd,
#if defined(USE_INKSO1) || defined(USE_INKSO2)
					heat_ptr + i + ((sizet-1)%2) * sizex,
#else 
					&heat(i, sizet-1), 
#endif
					sizeof(double)
					) == -1){
				perror("Fail writting in the file ");
				return EXIT_FAILURE;
			}
		}
	}else{
		perror("Fail opening the file ");
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

