#include <isl/options.h>

#include "options.h"

struct isl_arg_choice starshaping_strategy_choice[] = {
	{"none", STARSHAPING_NONE},
	{"simple", STARSHAPING_SIMPLE_HULL},
	{"convex", STARSHAPING_CONVEX_HULL},
	{"exact", STARSHAPING_EXACT},
	{0}
};

struct isl_arg_choice basis_completion_choice[] = {
	{"mapping", COMPLETE_MAPPING},
	{"lattice", COMPLETE_LATTICE},
	{0}
};

ISL_ARGS_START(struct lattifold_options, lattifold_options_args)
ISL_ARG_CHILD(struct lattifold_options, isl, "isl", &isl_options_args, "isl options")
ISL_ARG_CHOICE(struct lattifold_options, starshaping, 0, "starshaping", \
	starshaping_strategy_choice, STARSHAPING_SIMPLE_HULL,
	"starshaping algorithm to use")
ISL_ARG_CHOICE(struct lattifold_options, completion, 0, "completion", \
	basis_completion_choice, COMPLETE_MAPPING,
	"basis completion algorithm to use")
ISL_ARGS_END

ISL_ARG_DEF(lattifold_options, struct lattifold_options, lattifold_options_args)

ISL_ARG_CTX_DEF(lattifold_options, struct lattifold_options, lattifold_options_args)
ISL_CTX_SET_CHOICE_DEF(lattifold_options, struct lattifold_options, lattifold_options_args, starshaping)
ISL_CTX_GET_CHOICE_DEF(lattifold_options, struct lattifold_options, lattifold_options_args, starshaping)
ISL_CTX_SET_CHOICE_DEF(lattifold_options, struct lattifold_options, lattifold_options_args, completion)
ISL_CTX_GET_CHOICE_DEF(lattifold_options, struct lattifold_options, lattifold_options_args, completion)
