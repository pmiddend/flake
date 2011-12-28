#define WEIGHTED_JACOBI

#include "float_handling.cl"
#include "positions.cl"

kernel void
jacobi(
	/* 0 */flake_argument_real const alpha,
	/* 1 */flake_argument_real const beta,
	/* 2 */int const buffer_width,
	/* 3 */global float const *rhs,
	/* 4 */global float const *boundary,
	/* 5 */global float const *x,
	/* 6 */global float *output)
{
	int2 const currentpos =
		(int2)(
			get_global_id(0),
			get_global_id(1));

	int const
		current_index =
			FLAKE_AT(buffer_width,currentpos),
		left_index =
			FLAKE_LEFT_OF(buffer_width,currentpos),
		right_index =
			FLAKE_RIGHT_OF(buffer_width,currentpos),
		top_index =
			FLAKE_TOP_OF(buffer_width,currentpos),
		bottom_index =
			FLAKE_BOTTOM_OF(buffer_width,currentpos);

	flake_real const
		center =
			x[current_index],
		left =
			mix(
				x[left_index],
				center,
				boundary[left_index] > 0.2f ? 1.0f : 0.0f),
		right =
			mix(
				x[right_index],
				center,
				boundary[right_index] > 0.2f ? 1.0f : 0.0f),
		top =
			mix(
				x[top_index],
				center,
				boundary[top_index] > 0.2f ? 1.0f : 0.0f),
		bottom =
			mix(
				x[bottom_index],
				center,
				boundary[bottom_index] > 0.2f ? 1.0f : 0.0f);

	flake_real const rhs_value =
		rhs[current_index];

	flake_real const result =
		(left + right + top + bottom + alpha * rhs_value) * beta;

	output[current_index] =
#ifndef WEIGHTED_JACOBI
		result;
#else
		FLAKE_REAL_LIT(1.0)/FLAKE_REAL_LIT(3.0) * center +
		FLAKE_REAL_LIT(2.0)/FLAKE_REAL_LIT(3.0) * result;
#endif
}
