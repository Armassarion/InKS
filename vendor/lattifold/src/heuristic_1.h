#ifndef __HEURISTIC_1_H_
#define __HEURISTIC_1_H_

#include "partial_basis.h"

#include <isl/vec.h>
#include <isl/set.h>

/// Complete mapping while staying into the orthogonal of orth kernel until the end
__isl_give isl_vec* heuristic_1(__isl_take isl_set* deltas, partial_basis **basis);

#endif
