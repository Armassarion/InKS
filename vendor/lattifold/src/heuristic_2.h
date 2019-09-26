#ifndef __HEURISTIC_2_H_
#define __HEURISTIC_2_H_

#include "partial_basis.h"

#include <isl/vec.h>
#include <isl/set.h>

/// Complete lattice
__isl_give isl_vec* heuristic_2(__isl_take isl_set* deltas, partial_basis **basis);

#endif
