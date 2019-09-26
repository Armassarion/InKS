#include "lattifold.h"

#include "partial_basis.h"
#include "heuristic_3.h"
#include "successive_modulo.h"

#include <assert.h>

#include <isl/space.h>
#include "isl_tools.h"

__isl_give lattifold_mapping* lattifold_mapping_from_deltas(__isl_take isl_set* deltas) {
	isl_mat *mapp = heuristic_3(isl_set_copy(deltas));
	lattifold_mapping* mapping = malloc(sizeof(lattifold_mapping));
	mapping->proj = multi_aff_from_mat(isl_set_get_space(deltas), mapp);
	mapping->mods = successive_modulo(deltas, isl_multi_aff_copy(mapping->proj));
	mapping->mods = isl_multi_pw_aff_coalesce(mapping->mods);
	return mapping;
}

__isl_give isl_multi_aff* lattifold_mapping_get_projection(__isl_keep lattifold_mapping* mapping) {
	if (mapping == NULL)
		return NULL;

	return isl_multi_aff_copy(mapping->proj);
}

__isl_give isl_multi_pw_aff* lattifold_mapping_get_moduli(__isl_keep lattifold_mapping* mapping) {
	if (mapping == NULL)
		return NULL;

	return isl_multi_pw_aff_copy(mapping->mods);
}

__isl_null lattifold_mapping* lattifold_mapping_free(__isl_take lattifold_mapping* mapping) {
	if (mapping == NULL)
		return NULL;

	isl_multi_aff_free(mapping->proj);
	isl_multi_pw_aff_free(mapping->mods);
	free(mapping);
	return NULL;
}
