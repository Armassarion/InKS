#ifndef __ISL_TOOLS_H_
#define __ISL_TOOLS_H_

#include <isl/space.h>
#include <isl/point.h>
#include <isl/vec.h>
#include <isl/mat.h>
#include <isl/aff.h>
#include <isl/set.h>
#include <isl/map.h>

/// Convert back and forth between isl_vec and isl_point
__isl_give isl_vec* vec_from_point(__isl_take isl_point* point);
__isl_give isl_point* point_from_vec(__isl_take isl_space* space, __isl_take isl_vec* vec);

/// Convert back and forth between isl_vec and isl_aff (scalar product)
__isl_give isl_vec* vec_from_aff(__isl_take isl_aff* aff);
__isl_give isl_aff* aff_from_vec(__isl_take isl_space* space, __isl_take isl_vec* vec);

/// Convert back and forth between isl_mat and isl_multi_aff (right matrix product)
__isl_give isl_mat* mat_from_multi_aff(__isl_take isl_multi_aff* maff);
__isl_give isl_multi_aff* multi_aff_from_mat(__isl_take isl_space* space, __isl_take isl_mat* mat);

/// Get (or set) a row (or a column) of an isl_mat in the form of a isl_vec
__isl_give isl_vec* mat_get_row(__isl_keep isl_mat* mat, unsigned int row);
__isl_give isl_vec* mat_get_col(__isl_keep isl_mat* mat, unsigned int col);
__isl_give isl_mat* mat_set_row(__isl_take isl_mat* mat, unsigned int row, __isl_take isl_vec* vec);
__isl_give isl_mat* mat_set_col(__isl_take isl_mat* mat, unsigned int col, __isl_take isl_vec* vec);

/// Reverse the order of the rows (or columns, or both) of a matrix
__isl_give isl_mat* mat_reverse_rows(__isl_take isl_mat* mat);
__isl_give isl_mat* mat_reverse_cols(__isl_take isl_mat* mat);
__isl_give isl_mat* mat_reverse(__isl_take isl_mat* mat);

/// From { [i,j,k] } build { [i,j,k] : i > 0 or j > 0 or k > 0 }
__isl_give isl_set* set_build_atleast_one_pos(__isl_take isl_space *space);

#endif
