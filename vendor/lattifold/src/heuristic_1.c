#include "heuristic_1.h"

#include <isl/space.h>
#include <isl/map.h>
#include "isl_tools.h"

/// Takes { [i,j,k] } and 1 and produce { [j,k,i] }
__isl_give isl_basic_map* basic_map_rotate_domain(__isl_take isl_basic_map* bmap, unsigned int count) {
	isl_space *space = isl_basic_map_get_space(bmap);
	space = isl_space_drop_dims(space, isl_dim_out, 0, isl_space_dim(space, isl_dim_out));
	isl_local_space *lspace = isl_local_space_from_space(isl_space_domain(isl_space_copy(space)));
	isl_multi_aff *maff = isl_multi_aff_zero(space);
	unsigned int n = isl_local_space_dim(lspace, isl_dim_set);
	for (unsigned int i = 0; i < n; ++i) {
		unsigned int k = (i+count) % n;
		isl_aff *aff = isl_aff_var_on_domain(isl_local_space_copy(lspace), isl_dim_set, k);
		maff = isl_multi_aff_flat_range_product(maff, isl_multi_aff_from_aff(aff));
		isl_id *id = isl_multi_aff_get_dim_id(maff, isl_dim_in, k);
		isl_multi_aff_set_dim_id(maff, isl_dim_out, i, id);
	}
	return isl_basic_map_apply_domain(bmap, isl_basic_map_from_multi_aff(maff));
}

/// Same as coefficients except:
/// - unwraped
/// - parameters on right hand side
/// - constant on right hand side of parameters
__isl_give isl_basic_map* coefficients(__isl_take isl_set *set) {
	isl_basic_map *bmap = isl_basic_set_unwrap(isl_set_coefficients(set));
	bmap = basic_map_rotate_domain(bmap, 1);
	bmap = isl_basic_map_reverse(bmap);
	return bmap;
}

__isl_give isl_set* set_build_unimod_mapping_coefficients(__isl_take isl_space *space, __isl_take isl_mat* mat) {
	isl_ctx *ctx = isl_mat_get_ctx(mat);
	unsigned int rows = isl_mat_rows(mat);
	unsigned int cols = isl_mat_cols(mat);
	isl_space *domain = isl_space_set_alloc(ctx, 0, rows);
	if (isl_space_has_tuple_id(space, isl_dim_set))
	domain = isl_space_set_tuple_id(domain, isl_dim_set, isl_space_get_tuple_id(space, isl_dim_set));
	isl_multi_aff *maff = multi_aff_from_mat(isl_space_copy(domain), isl_mat_transpose(mat));
	isl_pw_multi_aff *pwmaff = isl_pw_multi_aff_from_multi_aff(maff);
	isl_set *set = set_build_atleast_one_pos(domain);
	pwmaff = isl_pw_multi_aff_intersect_domain(pwmaff, set);
	return isl_map_range(isl_map_from_pw_multi_aff(pwmaff));
}

__isl_give isl_vec* heuristic_1(__isl_take isl_set* deltas, partial_basis **pbasis) {
	/*
	fprintf(stderr, "==== Heuristic 1 ====\n");
	fprintf(stderr, "input basis:\n");
	partial_basis_dump(*pbasis);
	*/
	isl_space *space = isl_set_get_space(deltas);

	partial_basis *basis = partial_basis_copy(*pbasis);
	unsigned int n = partial_basis_dim(basis);
	unsigned int nmapp = partial_basis_mapping_dim(basis);
	unsigned int nlatt = partial_basis_lattice_dim(basis);
	
	isl_mat *mapp = partial_basis_mapping(basis);
	mapp = isl_mat_drop_rows(mapp, n-nlatt, nlatt);
	mapp = isl_mat_drop_rows(mapp, 0, nmapp);
	isl_set *unimod = set_build_unimod_mapping_coefficients(space, mapp);
/*
	fprintf(stderr, "search space:\n");
	isl_set_dump(unimod);
*/
/******/
	isl_basic_map *bmap = coefficients(deltas);
	//printf("\n-----%s\n\n", isl_basic_map_to_str(bmap));
	isl_map *map = isl_map_intersect_domain(isl_map_from_basic_map(bmap), isl_set_copy(unimod));
	isl_set *bmin = isl_set_lexmin(isl_map_range(isl_map_copy(map)));
	isl_point *point = isl_set_sample_point(isl_set_apply(bmin, isl_map_reverse(map)));
	if (isl_point_is_void(point)){
		//printf("%s\n", isl_point_to_str(point));
		//printf("The previous \"unbounded optimum\" error was expected.\nIf there are more of this error, please, create an issue.\n\n");
		point = isl_set_sample_point(unimod);
	}else
		isl_set_free(unimod);
	isl_vec *vec = vec_from_point(point);
/*****/
	*pbasis = partial_basis_add_mapping_vec(*pbasis, isl_vec_copy(vec));
/*
	fprintf(stderr, "output basis:\n");
	partial_basis_dump(*pbasis);
*/
	return vec;
}
