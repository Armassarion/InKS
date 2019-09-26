#include "starshape.h"

/* TODO
__isl_give isl_basic_set* basic_set_starshape_exact(__isl_take isl_basic_set* deltas) {
	isl_constraint_list *clist = isl_basic_set_get_constraint_list(deltas);
	unsigned int n = isl_constraint_list_
}
*/

__isl_give isl_basic_set* basic_set_starshape_simple_hull(__isl_take isl_basic_set* deltas) {
	isl_set *set = isl_set_from_basic_set(deltas);
	isl_space *space = isl_basic_set_get_space(deltas);
	space = isl_space_drop_dims(space, isl_dim_param, 0, isl_space_dim(space, isl_dim_param));
	isl_point *pnt = isl_point_zero(space);
	isl_set *pntset = isl_set_from_point(pnt);
	pntset = isl_set_intersect_params(pntset, isl_set_params(isl_set_copy(set)));
	return isl_set_simple_hull(isl_set_union(set, pntset));
}

__isl_give isl_basic_set* basic_set_starshape_convex_hull(__isl_take isl_basic_set* deltas) {
	isl_set *set = isl_set_from_basic_set(deltas);
	isl_space *space = isl_basic_set_get_space(deltas);
	space = isl_space_drop_dims(space, isl_dim_param, 0, isl_space_dim(space, isl_dim_param));
	isl_point *pnt = isl_point_zero(space);
	isl_set *pntset = isl_set_from_point(pnt);
	pntset = isl_set_intersect_params(pntset, isl_set_params(isl_set_copy(set)));
	return isl_set_convex_hull(isl_set_union(set, pntset));
}

__isl_give isl_set* basic_set_starshape(__isl_take isl_basic_set* deltas) {
	return isl_set_from_basic_set(basic_set_starshape_simple_hull(deltas));
}

isl_stat starshape_foreach_basic_set(__isl_take isl_basic_set* bset, void* user) {
	isl_set** acc = user;
	isl_set *set = basic_set_starshape(bset);
	*acc = isl_set_union(*acc, set);
	return isl_stat_ok;
}

__isl_give isl_set* set_starshape(__isl_take isl_set* deltas) {
	isl_set *acc = isl_set_empty(isl_set_get_space(deltas));
	isl_set_foreach_basic_set(deltas, &starshape_foreach_basic_set, &acc);
	return acc;
}
