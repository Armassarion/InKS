#include "heuristic_2.h"

#include <isl/map.h>
#include "isl_tools.h"

/// From bmap { [i,j] -> [o0] : c } and aff { [i,j] -> (f(i,j)) : } build { [i,j] -> [o0] : c and o0 > f(i,j) }
static __isl_give isl_basic_map* basic_map_add_aff(__isl_take isl_basic_map* bmap, __isl_take isl_aff* aff) {
	isl_basic_map* bmap2 = isl_basic_map_from_aff(aff);
	isl_space *space = isl_basic_map_get_space(bmap2);
	space = isl_space_range(space);
	space = isl_space_map_from_domain_and_range(space, isl_space_copy(space));
	isl_basic_map* bmap3 = isl_basic_map_universe(space);
	bmap3 = isl_basic_map_order_gt(bmap3, isl_dim_out, 0, isl_dim_in, 0);
	bmap2 = isl_basic_map_apply_range(bmap2, bmap3);
	return isl_basic_map_intersect(bmap, bmap2);
}

/// From space { [i,j,k] } builds basic_map { [i,j,k] -> [m] : m > i,j,k }
__isl_give isl_basic_map* build_max_norm(__isl_take isl_space* space) {
	unsigned int n = isl_space_dim(space, isl_dim_set);
	isl_local_space *ls = isl_local_space_from_space(isl_space_copy(space));
	space = isl_space_from_domain(space);
	space = isl_space_add_dims(space, isl_dim_out, 1);
	isl_basic_map *bmap = isl_basic_map_universe(space);
	for (int i = 0; i < n; ++i) {
		isl_aff *aff = isl_aff_var_on_domain(isl_local_space_copy(ls), isl_dim_set, i);
		bmap = basic_map_add_aff(bmap, isl_aff_copy(aff));
		bmap = basic_map_add_aff(bmap, isl_aff_neg(aff));
	}
	isl_local_space_free(ls);
	return bmap;
}

/// From space { [i,j,k] } builds basic_map { [i,j,k] -> [m] : m > i+j+k,i+j-k,... }
__isl_give isl_basic_map* build_sum_norm(__isl_take isl_space* space) {
	unsigned int n = isl_space_dim(space, isl_dim_set);
	isl_local_space *ls = isl_local_space_from_space(isl_space_copy(space));
	space = isl_space_from_domain(space);
	space = isl_space_add_dims(space, isl_dim_out, 1);
	isl_basic_map *bmap = isl_basic_map_universe(space);
	for (int k = 0; k < (1 << n); ++k) {
		isl_aff *aff = isl_aff_zero_on_domain(isl_local_space_copy(ls));
		for (int i = 0; i < n; ++i)
			if (k & (1 << i))
				aff = isl_aff_sub(aff, isl_aff_var_on_domain(isl_local_space_copy(ls), isl_dim_set, i));
			else
				aff = isl_aff_add(aff, isl_aff_var_on_domain(isl_local_space_copy(ls), isl_dim_set, i));
		bmap = basic_map_add_aff(bmap, aff);
	}
	isl_local_space_free(ls);
	return bmap;
}

__isl_give isl_basic_map* build_norm(__isl_take isl_space* space) {
	isl_basic_map *max_norm = build_max_norm(isl_space_copy(space));
	isl_basic_map *sum_norm = build_sum_norm(isl_space_copy(space));

	isl_multi_aff *maff = isl_multi_aff_identity(isl_space_map_from_set(space));
	isl_basic_map *norm = isl_basic_map_from_multi_aff(isl_multi_aff_neg(maff));

	norm = isl_basic_map_flat_range_product(sum_norm, norm);
	norm = isl_basic_map_flat_range_product(max_norm, norm);

	return norm;
}

__isl_give isl_set* set_build_unimod_lattice_coefficients(__isl_take isl_space *space, __isl_take isl_mat* mat) {
	isl_ctx *ctx = isl_mat_get_ctx(mat);
	unsigned int rows = isl_mat_rows(mat);
	unsigned int cols = isl_mat_cols(mat);
	isl_space *domain = isl_space_set_alloc(ctx, 0, cols);
	if (isl_space_has_tuple_id(space, isl_dim_set))
	domain = isl_space_set_tuple_id(domain, isl_dim_set, isl_space_get_tuple_id(space, isl_dim_set));
	isl_multi_aff *maff = multi_aff_from_mat(isl_space_copy(domain), mat);
	isl_pw_multi_aff *pwmaff = isl_pw_multi_aff_from_multi_aff(maff);
	isl_set *set = set_build_atleast_one_pos(domain);
	pwmaff = isl_pw_multi_aff_intersect_domain(pwmaff, set);
	return isl_map_range(isl_map_from_pw_multi_aff(pwmaff));
}

__isl_give isl_vec* heuristic_2(__isl_take isl_set* deltas, partial_basis **pbasis) {
	//fprintf(stderr, "==== Heuristic 2 ====\n");
	//fprintf(stderr, "input basis:\n");
	//partial_basis_dump(*pbasis);

	isl_space *space = isl_set_get_space(deltas);

	partial_basis *basis = partial_basis_copy(*pbasis);
	unsigned int n = partial_basis_dim(basis);
	unsigned int nmapp = partial_basis_mapping_dim(basis);
	unsigned int nlatt = partial_basis_lattice_dim(basis);

	isl_mat* latt = partial_basis_lattice(basis);
	latt = isl_mat_drop_cols(latt, n-nlatt, nlatt);
	latt = isl_mat_drop_cols(latt, 0, nmapp);
	isl_set *unimod = set_build_unimod_lattice_coefficients(isl_space_copy(space), latt);

	//fprintf(stderr, "search space:\n");
	//isl_set_dump(unimod);

	isl_basic_map *norm = build_norm(space);
	deltas = isl_set_complement(deltas);
	deltas = isl_set_intersect(deltas, unimod);
	deltas = isl_set_apply(deltas, isl_map_from_basic_map(isl_basic_map_copy(norm)));
	deltas = isl_set_lexmin(deltas);
	deltas = isl_set_apply(deltas, isl_map_from_basic_map(isl_basic_map_reverse(norm)));
	if (isl_set_is_empty(deltas))
		return NULL;
	isl_point *pnt = isl_set_sample_point(deltas);
	isl_vec *vec = vec_from_point(pnt);

	*pbasis = partial_basis_add_lattice_vec(*pbasis, isl_vec_copy(vec));

	//fprintf(stderr, "output basis:\n");
	//partial_basis_dump(*pbasis);

	return vec;
}
