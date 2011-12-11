#define WEIGHTED_JACOBI

#include "float_handling.cl"
#include "positions.cl"

sampler_t const absolute_clamping_nearest =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_NEAREST;

/**
	Solve the special equation

	Ax=0

	where A is the Laplace operator.
*/
kernel void
jacobi(
	/* 0 */float const alpha,
	/* 1 */float const beta,
	/* 2 */global read_only image2d_t rhs,
	/* 3 */global read_only image2d_t boundary,
	/* 4 */global read_only image2d_t x,
	/* 5 */global write_only image2d_t output)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	flake_real const
		left_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_left).x,
		right_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_right).x,
		top_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_bottom).x;

	flake_real
		center =
			FLAKE_READ_IMAGE_FUNCTION(
				x,
				absolute_clamping_nearest,
				position).x,
		left =
			mix(
				FLAKE_READ_IMAGE_FUNCTION(
					x,
					absolute_clamping_nearest,
					position + pos_left).x,
				center,
				left_boundary),
		right =
			mix(
				FLAKE_READ_IMAGE_FUNCTION(
					x,
					absolute_clamping_nearest,
					position + pos_right).x,
				center,
				right_boundary),
		top =
			mix(
				FLAKE_READ_IMAGE_FUNCTION(
					x,
					absolute_clamping_nearest,
					position + pos_top).x,
				center,
				top_boundary),
		bottom =
			mix(
				FLAKE_READ_IMAGE_FUNCTION(
					x,
					absolute_clamping_nearest,
					position + pos_bottom).x,
				center,
				bottom_boundary);

	flake_real const b_value =
		FLAKE_READ_IMAGE_FUNCTION(
				rhs,
				absolute_clamping_nearest,
				position).x;

	FLAKE_WRITE_IMAGE_FUNCTION(
		output,
		position,
#ifndef WEIGHTED_JACOBI
		(flake_real4)((left + right + top + bottom + alpha * b_value) * beta,FLAKE_REAL_LIT(0.0),FLAKE_REAL_LIT(0.0),FLAKE_REAL_LIT(0.0)));
#else
		//(flake_real4)(FLAKE_REAL_LIT(1.0)/FLAKE_REAL_LIT(3.0) * center + FLAKE_REAL_LIT(2.0) / FLAKE_REAL_LIT(3.0) * (left + right + top + bottom + alpha * b_value) * beta,FLAKE_REAL_LIT(0.0),FLAKE_REAL_LIT(0.0),FLAKE_REAL_LIT(0.0)));
		(flake_real4)(
			FLAKE_REAL_LIT(1.0)/FLAKE_REAL_LIT(3.0) * center +
			FLAKE_REAL_LIT(2.0)/FLAKE_REAL_LIT(3.0) * (left + right + top + bottom + alpha * b_value) * beta,
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0)));
#endif
}
