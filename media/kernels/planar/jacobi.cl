#define WEIGHTED_JACOBI

#include "positions.cl"

kernel void
jacobi(
	/* 0 */float const alpha,
	/* 1 */float const beta,
	/* 2 */int const buffer_width,
	/* 3 */global float const *rhs,
	/* 4 */global float const *boundary,
	/* 5 */global float const *x,
	/* 6 */global float *output)
{
	int3 const currentpos =
		(int3)(
			get_global_id(0),
			get_global_id(1),
			get_global_id(2));

	int const
		current_index =
			FLAKE_VOLUME_AT(buffer_width,currentpos),
		left_index =
			FLAKE_VOLUME_LEFT_OF(buffer_width,currentpos),
		right_index =
			FLAKE_VOLUME_RIGHT_OF(buffer_width,currentpos),
		top_index =
			FLAKE_VOLUME_TOP_OF(buffer_width,currentpos),
		bottom_index =
			FLAKE_VOLUME_BOTTOM_OF(buffer_width,currentpos),
		back_index =
			FLAKE_VOLUME_BACK_OF(buffer_width,currentpos),
		front_index =
			FLAKE_VOLUME_FRONT_OF(buffer_width,currentpos);

	float const
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
				boundary[bottom_index] > 0.2f ? 1.0f : 0.0f),
		back =
			mix(
				x[back_index],
				center,
				boundary[back_index] > 0.2f ? 1.0f : 0.0f),
		front =
			mix(
				x[front_index],
				center,
				boundary[front_index] > 0.2f ? 1.0f : 0.0f);

	float const rhs_value =
		rhs[current_index];

	float const result =
		(left + right + top + bottom + back + front + alpha * rhs_value) * beta;

	output[current_index] =
#ifndef WEIGHTED_JACOBI
		result;
#else
		1.0f/3.0f * center +
		2.0f/3.0f * result;
#endif
}
