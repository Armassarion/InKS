#ifndef __HEURISTIC_3_H_
#define __HEURISTIC_3_H_

#include "lattifold.h"

#include <isl/set.h>
#include <isl/aff.h>

/// Compute mapping using Heuristic 2 then Heuristic 1
isl_mat* heuristic_3(__isl_take isl_set* deltas);

#endif
