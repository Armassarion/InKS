#ifndef __LATTIFOLD_H_
#define __LATTIFOLD_H_

#include <isl/set.h>
#include <isl/aff.h>

struct lattifold_mapping {
	isl_multi_aff* proj;
	isl_multi_pw_aff* mods;
};

typedef struct lattifold_mapping lattifold_mapping;

__isl_give lattifold_mapping* lattifold_mapping_from_deltas(__isl_take isl_set* deltas);
__isl_give isl_multi_aff* lattifold_mapping_get_projection(__isl_keep lattifold_mapping* mapping);
__isl_give isl_multi_pw_aff* lattifold_mapping_get_moduli(__isl_keep lattifold_mapping* mapping);
__isl_null lattifold_mapping* lattifold_mapping_free(__isl_take lattifold_mapping* maping);

#endif
