#pragma once

#include "array2d.hpp"
#include "heat_equation.loops.inks.hpp"

void inks_heat ( Array2D& heat, size_t dimx, size_t dimt )
{
	init(heat, dimx);
	for ( size_t t = 1; t < dimt; ++t ) {
		bord(heat, 0, t);
		#pragma inks for loop_inner inner (heat) inner_option
		loop_inner(heat, t, dimx, dimt);
		/*
		for ( int x = 1; x < dimx-1; ++x ) {
			inner(heat, x, t);
		}
		*/
		bord(heat, dimx-1, t);
	}
}
