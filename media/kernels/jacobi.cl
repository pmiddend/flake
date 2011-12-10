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
			left_boundary *
			center +
			(FLAKE_REAL_LIT(1.0) - left_boundary) *
			FLAKE_READ_IMAGE_FUNCTION(
				x,
				absolute_clamping_nearest,
				position + pos_left).x,
		right =
			right_boundary *
			center +
			(FLAKE_REAL_LIT(1.0) - right_boundary) *
			FLAKE_READ_IMAGE_FUNCTION(
				x,
				absolute_clamping_nearest,
				position + pos_right).x,
		top =
			top_boundary *
			center +
			(FLAKE_REAL_LIT(1.0) - top_boundary) *
			FLAKE_READ_IMAGE_FUNCTION(
				x,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom =
			bottom_boundary *
			center +
			(FLAKE_REAL_LIT(1.0) - bottom_boundary) *
			FLAKE_READ_IMAGE_FUNCTION(
				x,
				absolute_clamping_nearest,
				position + pos_bottom).x;

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
		(flake_real4)(FLAKE_REAL_LIT(1.0)/FLAKE_REAL_LIT(3.0) * center + FLAKE_REAL_LIT(2.0) / FLAKE_REAL_LIT(3.0) * (left + right + top + bottom + alpha * b_value) * beta,FLAKE_REAL_LIT(0.0),FLAKE_REAL_LIT(0.0),FLAKE_REAL_LIT(0.0)));
#endif
}
