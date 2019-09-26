#include "partial_basis.h"

#include "isl_tools.h"

struct partial_basis {
	isl_mat *mapp; // Line matrix, first vector is at the top
	isl_mat *latt; // Column matrix, first vector is on the right
};

__isl_give isl_mat* mat_mapping_complete(__isl_take isl_mat* part) {
	isl_mat *comp = NULL;
	part = isl_mat_left_hermite(part, 0, NULL, &comp);
	isl_mat_free(part);
	return comp;
}

__isl_give isl_mat* mat_lattice_complete(__isl_take isl_mat* part) {
	isl_mat *comp = NULL;
	part = mat_reverse(part);
	part = isl_mat_transpose(part);
	part = isl_mat_left_hermite(part, 0, NULL, &comp);
	isl_mat_free(part);
	comp = isl_mat_transpose(comp);
	comp = mat_reverse(comp);
	return comp;
}

__isl_give isl_mat* mat_mapping_from_lattice(__isl_take isl_mat* latt) {
	isl_mat *mapp = NULL;
	latt = mat_reverse(latt);
	latt = isl_mat_transpose(latt);
	latt = isl_mat_left_hermite(latt, 0, &mapp, NULL);
	isl_mat_free(latt);
	mapp = isl_mat_transpose(mapp);
	mapp = mat_reverse(mapp);
	return mapp;
}

__isl_give isl_mat* mat_lattice_from_mapping(__isl_take isl_mat* mapp) {
	isl_mat *latt = NULL;
	mapp = isl_mat_left_hermite(mapp, 0, &latt, NULL);
	isl_mat_free(mapp);
	return latt;
}

__isl_give partial_basis* partial_basis_alloc(isl_ctx *ctx, unsigned int n) {
	partial_basis *basis = malloc(sizeof(partial_basis));
	basis->mapp = isl_mat_alloc(ctx, 0, n);
	basis->latt = isl_mat_alloc(ctx, n, 0);
	return basis;
}

__isl_give partial_basis* partial_basis_copy(__isl_keep partial_basis* other) {
	partial_basis *pbase = malloc(sizeof(partial_basis));
	pbase->mapp = isl_mat_copy(other->mapp);
	pbase->latt = isl_mat_copy(other->latt);
	return pbase;
}

void partial_basis_dump(__isl_keep partial_basis* basis) {
	isl_mat_dump(basis->mapp);
	isl_mat_dump(basis->latt);
	/*
	isl_mat *syndrome = isl_mat_product(isl_mat_copy(basis->mapp), isl_mat_copy(basis->latt));
	isl_mat_dump(syndrome);
	isl_mat_free(syndrome);
	*/
}

__isl_null __isl_give partial_basis* partial_basis_free(__isl_take partial_basis* basis) {
	if (basis != NULL) {
		isl_mat_free(basis->mapp);
		isl_mat_free(basis->latt);
		free(basis);
	}
	return NULL;
}

__isl_give isl_mat* partial_basis_mapping(__isl_take partial_basis* basis) {
	basis = partial_basis_complete(basis);
	isl_mat *mapp = basis->mapp;
	basis->mapp = NULL;
	partial_basis_free(basis);
	return mapp;
}

__isl_give isl_mat* partial_basis_lattice(__isl_take partial_basis* basis) {
	basis = partial_basis_complete(basis);
	isl_mat *latt = basis->latt;
	basis->latt = NULL;
	partial_basis_free(basis);
	return latt;
}

__isl_give unsigned int partial_basis_dim(__isl_keep partial_basis* basis) {
	return isl_mat_cols(basis->mapp);
}

__isl_give unsigned int partial_basis_mapping_dim(__isl_keep partial_basis* basis) {
	return isl_mat_rows(basis->mapp);
}

__isl_give unsigned int partial_basis_lattice_dim(__isl_keep partial_basis* basis) {
	return isl_mat_cols(basis->latt);
}

__isl_give partial_basis* partial_basis_add_mapping_vec(__isl_take partial_basis* basis, __isl_take isl_vec* vec) {
	basis->mapp = isl_mat_vec_concat(basis->mapp, vec);
	return basis;
}

__isl_give partial_basis* partial_basis_add_lattice_vec(__isl_take partial_basis* basis, __isl_take isl_vec* vec) {
	basis->latt = isl_mat_transpose(basis->latt);
	basis->latt = mat_reverse_rows(basis->latt);
	basis->latt = isl_mat_vec_concat(basis->latt, vec);
	basis->latt = mat_reverse_rows(basis->latt);
	basis->latt = isl_mat_transpose(basis->latt);
	return basis;
}

/* Incorrect, choose your side!
__isl_give partial_basis* partial_basis_complete(__isl_take partial_basis* basis) {
	fprintf(stderr, "Before:\n");
	partial_basis_dump(basis);

	// Mappings
	isl_mat *latt_comp = mat_lattice_complete(isl_mat_copy(basis->latt));
	isl_mat *mapp_dest = mat_mapping_from_lattice(isl_mat_copy(basis->latt));

	// Lattices
	isl_mat *mapp_comp = mat_mapping_complete(isl_mat_copy(basis->mapp));
	isl_mat *latt_dest = mat_lattice_from_mapping(isl_mat_copy(basis->mapp));

	// Basis changement
	isl_mat *mapp_chgmt = isl_mat_product(basis->mapp, latt_comp);
	isl_mat *latt_chgmt = isl_mat_product(mapp_comp, basis->latt);
	mapp_chgmt = mat_mapping_complete(mapp_chgmt);
	latt_chgmt = mat_lattice_complete(latt_chgmt);

	fprintf(stderr, "Changements:\n");
	isl_mat_dump(mapp_chgmt);
	isl_mat_dump(latt_chgmt);

	// Apply basis changement
	basis->mapp = isl_mat_product(mapp_chgmt, mapp_dest);
	basis->latt = isl_mat_product(latt_dest, latt_chgmt);

	fprintf(stderr, "After:\n");
	partial_basis_dump(basis);

	return basis;
}
*/

__isl_give partial_basis* partial_basis_complete(__isl_take partial_basis* basis) {
	// Mappings
	isl_mat *latt_comp = mat_lattice_complete(isl_mat_copy(basis->latt));
	isl_mat *mapp_dest = mat_mapping_from_lattice(basis->latt);

	// Basis changement
	isl_mat *mapp_chgmt = isl_mat_product(basis->mapp, latt_comp);
	mapp_chgmt = mat_mapping_complete(mapp_chgmt);

	// Apply basis changement
	basis->mapp = isl_mat_product(mapp_chgmt, mapp_dest);
	basis->latt = isl_mat_right_inverse(isl_mat_copy(basis->mapp));

	return basis;
}
