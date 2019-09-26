#include "heuristic_3.h"

#include "starshape.h"
#include "heuristic_1.h"
#include "heuristic_2.h"

#include <isl/constraint.h>
#include "isl_tools.h"

static __isl_give isl_set* slice(__isl_take isl_set* deltas, __isl_take isl_vec* vec) {
	isl_aff *aff = aff_from_vec(isl_set_get_space(deltas), vec);
	isl_constraint *c = isl_equality_from_aff(aff);
	deltas = isl_set_add_constraint(deltas, c);
	return isl_set_coalesce(deltas);
}

static __isl_give isl_set* extrude(__isl_take isl_set* deltas, __isl_take isl_vec* vec) {
	isl_aff *aff = aff_from_vec(isl_set_get_space(deltas), vec);
	isl_basic_map *lhs = isl_basic_map_from_aff(aff);
	isl_basic_map *rhs = isl_basic_map_reverse(isl_basic_map_copy(lhs));
	isl_basic_map *bmap = isl_basic_map_apply_range(lhs, rhs);
	deltas = isl_set_apply(deltas, isl_map_from_basic_map(bmap));
	return isl_set_coalesce(deltas);
}

/// Compute mapping using Heuristic 2 then Heuristic 1
isl_mat* heuristic_3(__isl_take isl_set* deltas) {
	isl_ctx *ctx = isl_set_get_ctx(deltas);
	unsigned int n = isl_set_dim(deltas, isl_dim_set);
	partial_basis *basis = partial_basis_alloc(ctx, n);

	// Heuristic 2
	{
		isl_set *ssdeltas = set_starshape(isl_set_copy(deltas));
		if (isl_set_is_equal(deltas, ssdeltas) == isl_bool_false) {
			//fprintf(stderr, "=== Star shaping ===\n");
			//isl_set_dump(ssdeltas);
		}
		ssdeltas = isl_set_project_out(deltas, isl_dim_param, 0, isl_set_dim(ssdeltas, isl_dim_param));
		isl_vec *vec = NULL;
		while (partial_basis_lattice_dim(basis) + partial_basis_mapping_dim(basis) < n && (vec = heuristic_2(isl_set_copy(ssdeltas), &basis)))
			ssdeltas = extrude(ssdeltas, vec);
		isl_set_free(ssdeltas);
	}

	// Heuristic 1
	{
		isl_vec *vec = NULL;
		while (partial_basis_lattice_dim(basis) + partial_basis_mapping_dim(basis) < n && (vec = heuristic_1(isl_set_copy(deltas), &basis))){
			deltas = slice(deltas, vec);
		}
	}

	isl_set_free(deltas);
	return partial_basis_mapping(basis);
}
