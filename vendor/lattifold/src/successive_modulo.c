#include "successive_modulo.h"

#include <isl/ctx.h>
#include <isl/constraint.h>
#include <isl/set.h>
#include <isl/map.h>

__isl_give isl_set* slice(__isl_take isl_set* deltas, __isl_take isl_aff* ortho) {
	isl_constraint* c = isl_equality_from_aff(ortho);
	deltas = isl_set_add_constraint(deltas, c);
	return isl_set_coalesce(deltas);
}

__isl_give isl_pw_aff* modulo(__isl_take isl_set* deltas, __isl_take isl_aff* projection) {
	isl_ctx* ctx = isl_set_get_ctx(deltas);
	isl_set* range = isl_set_apply(deltas, isl_map_from_aff(projection));
	isl_pw_aff* pwaff = isl_set_dim_max(range, 0);
	isl_set* all = isl_set_universe(isl_pw_aff_get_domain_space(pwaff));
	isl_pw_aff* one = isl_pw_aff_val_on_domain(all, isl_val_one(ctx));
	return isl_pw_aff_add(pwaff, one);
}

__isl_give isl_multi_pw_aff* successive_modulo(__isl_take isl_set* deltas, __isl_take isl_multi_aff* projection) {
	unsigned int n = isl_multi_aff_dim(projection, isl_dim_out);
	isl_space* space = isl_set_get_space(deltas);
	space = isl_space_drop_dims(space, isl_dim_set, 0, isl_space_dim(space, isl_dim_set));
	isl_multi_pw_aff* mpwaff = isl_multi_pw_aff_zero(space);
	for (unsigned int i = 0; i < n; ++i) {
		isl_aff* proj = isl_multi_aff_get_aff(projection, i);
		isl_pw_aff* pwaff = modulo(isl_set_copy(deltas), isl_aff_copy(proj));
		mpwaff = isl_multi_pw_aff_flat_range_product(mpwaff, isl_multi_pw_aff_from_pw_aff(pwaff));
		deltas = slice(deltas, proj);
	}
	isl_set_free(deltas);
	return mpwaff;
}
