#ifndef __SUCCESSIVE_MODULO_H_
#define __SUCCESSIVE_MODULO_H_

#include <isl/aff.h>
#include <isl/space.h>

__isl_take isl_multi_pw_aff* successive_modulo(__isl_take isl_set* deltas, __isl_take isl_multi_aff* projection);

#endif
