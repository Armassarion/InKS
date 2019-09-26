#ifndef __OPTIONS_H_
#define __OPTIONS_H_

#include <isl/arg.h>

enum starshaping_strategy {
	STARSHAPING_NONE,
	STARSHAPING_SIMPLE_HULL,
	STARSHAPING_CONVEX_HULL,
	STARSHAPING_EXACT
};

enum basis_completion {
	COMPLETE_MAPPING,
	COMPLETE_LATTICE
};

struct lattifold_options {
	struct isl_options* isl;
	enum starshaping_strategy starshaping;
	enum basis_completion completion;
};

ISL_ARG_DECL(lattifold_options, struct lattifold_options, lattifold_options_args)

#endif
