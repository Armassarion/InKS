#include "isl_tools.h"

#include <assert.h>

/* ==== isl_vec <-> isl_point ==== */
static unsigned int isl_point_dim(__isl_keep isl_point *point, enum isl_dim_type type) {
	isl_space *space = isl_point_get_space(point);
	unsigned int n = isl_space_dim(space, type);
	isl_space_free(space);
	return n;
}

__isl_give isl_vec* vec_from_point(__isl_take isl_point* point) {
	isl_ctx *ctx = isl_point_get_ctx(point);
	unsigned int size = isl_point_dim(point, isl_dim_set);
	isl_vec *vec = isl_vec_alloc(ctx, size);
	for (unsigned int i = 0; i < size; ++i) {
		isl_val *val = isl_point_get_coordinate_val(point, isl_dim_set, i);
		vec = isl_vec_set_element_val(vec, i, val);
	}
	isl_point_free(point);
	return vec;
}

__isl_give isl_point* point_from_vec(__isl_take isl_space* space, __isl_take isl_vec* vec) {
	assert(isl_space_dim(space, isl_dim_set) == isl_vec_size(vec));
	isl_point *point = isl_point_zero(space);
	unsigned int size = isl_vec_size(vec);
	for (unsigned int i = 0; i < size; ++i) {
		isl_val *val = isl_vec_get_element_val(vec, i);
		point = isl_point_set_coordinate_val(point, isl_dim_set, i, val);
	}
	isl_vec_free(vec);
	return point;
}

/* ==== isl_vec <-> isl_aff ==== */
__isl_give isl_vec* vec_from_aff(__isl_take isl_aff* aff) {
	isl_ctx *ctx = isl_aff_get_ctx(aff);
	unsigned int size = isl_aff_dim(aff, isl_dim_in);
	isl_vec *vec = isl_vec_alloc(ctx, size);
	for (unsigned int i = 0; i < size; ++i) {
		isl_val *val = isl_aff_get_coefficient_val(aff, isl_dim_in, i);
		vec = isl_vec_set_element_val(vec, i, val);
	}
	isl_aff_free(aff);
	return vec;
}

__isl_give isl_aff* aff_from_vec(__isl_take isl_space* space, __isl_take isl_vec* vec) {
	unsigned int size = isl_vec_size(vec);
	isl_aff *aff = isl_aff_zero_on_domain(isl_local_space_from_space(space));
	for (unsigned int i = 0; i < size; ++i) {
		isl_val *val = isl_vec_get_element_val(vec, i);
		aff = isl_aff_set_coefficient_val(aff, isl_dim_in, i, val);
	}
	isl_vec_free(vec);
	return aff;
}

/* ==== isl_mat <-> isl_multi_aff ==== */
__isl_give isl_mat* mat_from_multi_aff(__isl_take isl_multi_aff* maff) {
	isl_ctx *ctx = isl_multi_aff_get_ctx(maff);
	unsigned int rows = isl_multi_aff_dim(maff, isl_dim_out);
	unsigned int cols = isl_multi_aff_dim(maff, isl_dim_in);
	isl_mat *mat = isl_mat_alloc(ctx, rows, cols);
	for (unsigned int i = 0; i < rows; ++i) {
		isl_aff *aff = isl_multi_aff_get_aff(maff, i);
		for (unsigned int j = 0; j < cols; ++j) {
			isl_val *val = isl_aff_get_coefficient_val(aff, isl_dim_in, j);
			mat = isl_mat_set_element_val(mat, i, j, val);
		}
		isl_aff_free(aff);
	}
	isl_multi_aff_free(maff);
	return mat;
}

__isl_give isl_multi_aff* multi_aff_from_mat(__isl_take isl_space* space, __isl_take isl_mat* mat) {
	unsigned int rows = isl_mat_rows(mat);
	unsigned int cols = isl_mat_cols(mat);
	isl_multi_aff *maff = isl_multi_aff_zero(isl_space_from_domain(isl_space_copy(space)));
	isl_local_space *ldom = isl_local_space_from_space(space);
	for (unsigned int i = 0; i < rows; ++i) {
		isl_aff *aff = isl_aff_zero_on_domain(isl_local_space_copy(ldom));
		for (unsigned int j = 0; j < cols; ++j) {
			isl_val *val = isl_mat_get_element_val(mat, i, j);
			aff = isl_aff_set_coefficient_val(aff, isl_dim_in, j, val);
		}
		maff = isl_multi_aff_flat_range_product(maff, isl_multi_aff_from_aff(aff));
	}
	isl_mat_free(mat);
	if (isl_multi_aff_has_tuple_id(maff, isl_dim_in))
	maff = isl_multi_aff_set_tuple_id(maff, isl_dim_out, isl_multi_aff_get_tuple_id(maff, isl_dim_in));
	return maff;
}

/* ==== mat_reverse_{rows,cols} ==== */
__isl_give isl_mat* mat_reverse_rows(__isl_take isl_mat* mat) {
	unsigned int n = isl_mat_rows(mat);
	for (unsigned int i = 0; 2*i < n; ++i)
		mat = isl_mat_swap_rows(mat, i, n-i-1);
	return mat;
}

__isl_give isl_mat* mat_reverse_cols(__isl_take isl_mat* mat) {
	unsigned int n = isl_mat_cols(mat);
	for (unsigned int i = 0; 2*i < n; ++i)
		mat = isl_mat_swap_cols(mat, i, n-i-1);
	return mat;
}

__isl_give isl_mat* mat_reverse(__isl_take isl_mat* mat) {
	mat = mat_reverse_rows(mat);
	mat = mat_reverse_cols(mat);
	return mat;
}

/* ==== mat_{get,set}_{row,col} ==== */
__isl_give isl_vec* mat_get_row(__isl_keep isl_mat* mat, unsigned int row) {
	isl_ctx *ctx = isl_mat_get_ctx(mat);
	unsigned int n = isl_mat_cols(mat);
	isl_vec *vec = isl_vec_alloc(ctx, n);
	for (unsigned int i = 0; i < n; ++i) {
		isl_val *val = isl_mat_get_element_val(mat, row, i);
		vec = isl_vec_set_element_val(vec, i, val);
	}
	return vec;
}

__isl_give isl_vec* mat_get_col(__isl_keep isl_mat* mat, unsigned int col) {
	isl_ctx *ctx = isl_mat_get_ctx(mat);
	unsigned int n = isl_mat_rows(mat);
	isl_vec *vec = isl_vec_alloc(ctx, n);
	for (unsigned int i = 0; i < n; ++i) {
		isl_val *val = isl_mat_get_element_val(mat, i, col);
		vec = isl_vec_set_element_val(vec, i, val);
	}
	return vec;
}

__isl_give isl_mat* mat_set_row(__isl_take isl_mat* mat, unsigned int row, __isl_take isl_vec* vec) {
	assert(isl_mat_cols(mat) == isl_vec_size(vec));
	unsigned int n = isl_vec_size(vec);
	for (unsigned int i = 0; i < n; ++i) {
		isl_val *val = isl_vec_get_element_val(vec, i);
		mat = isl_mat_set_element_val(mat, row, i, val);
	}
	isl_vec_free(vec);
	return mat;
}

__isl_give isl_mat* mat_set_col(__isl_take isl_mat* mat, unsigned int col, __isl_take isl_vec* vec) {
	assert(isl_mat_rows(mat) == isl_vec_size(vec));
	unsigned int n = isl_vec_size(vec);
	for (unsigned int i = 0; i < n; ++i) {
		isl_val *val = isl_vec_get_element_val(vec, i);
		mat = isl_mat_set_element_val(mat, i, col, val);
	}
	isl_vec_free(vec);
	return mat;
}

/* ==== aff_{val,var}_on_domain ==== */
__isl_give isl_aff* aff_val_on_domain(__isl_take isl_space* space, __isl_take isl_val *val) {
	isl_local_space *ls = isl_local_space_from_space(space);
	return isl_aff_val_on_domain(ls, val);
}

__isl_give isl_aff* aff_var_on_domain(__isl_take isl_space* space, enum isl_dim_type type, unsigned pos) {
	isl_local_space *ls = isl_local_space_from_space(space);
	return isl_aff_var_on_domain(ls, type, pos);
}

/// From { [i,j,k] } build { [i,j,k] : i > 0 or j > 0 or k > 0 }
__isl_give isl_set* set_build_atleast_one_pos(__isl_take isl_space *space) {
	unsigned int n = isl_space_dim(space, isl_dim_set);
	isl_set *set = isl_set_empty(isl_space_copy(space));
	isl_local_space *lspace = isl_local_space_from_space(space);
	for (unsigned int i = 0; i < n; ++i) {
		isl_aff *aff = isl_aff_var_on_domain(isl_local_space_copy(lspace), isl_dim_set, i);
		isl_basic_set *bset = isl_aff_neg_basic_set(isl_aff_neg(aff));
		set = isl_set_union(set, isl_set_from_basic_set(bset));
	}
	return set;
}
