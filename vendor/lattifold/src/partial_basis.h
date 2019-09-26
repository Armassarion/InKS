#ifndef __PARTIAL_BASIS_H_
#define __PARTIAL_BASIS_H_

#include <isl/mat.h>

struct partial_basis;
typedef struct partial_basis partial_basis;

__isl_give partial_basis* partial_basis_alloc(isl_ctx *ctx, unsigned int n);
__isl_give partial_basis* partial_basis_copy(__isl_keep partial_basis* basis);
void partial_basis_dump(__isl_keep partial_basis *basis);
__isl_null __isl_give partial_basis* partial_basis_free(__isl_take partial_basis* basis);

/// Get {mapping,lattice} matrix
__isl_give isl_mat* partial_basis_mapping(__isl_take partial_basis* basis);
__isl_give isl_mat* partial_basis_lattice(__isl_take partial_basis* basis);

/// Get dimension of each partial mapping
__isl_give unsigned int partial_basis_dim(__isl_keep partial_basis* basis);
__isl_give unsigned int partial_basis_mapping_dim(__isl_keep partial_basis* basis);
__isl_give unsigned int partial_basis_lattice_dim(__isl_keep partial_basis* basis);

/// Add new {mapping,lattice} vector
__isl_give partial_basis* partial_basis_add_mapping_vec(__isl_take partial_basis* basis, __isl_take isl_vec* vec);
__isl_give partial_basis* partial_basis_add_lattice_vec(__isl_take partial_basis* basis, __isl_take isl_vec* vec);

/// Complete a partial basis
__isl_give partial_basis* partial_basis_complete(__isl_take partial_basis* basis);

#endif
