#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>
#include <flakelib/volume/global_size.cl>
#include <flakelib/volume/current_position.cl>
#include <flakelib/volume/von_neumann/load_neighbor_indices.cl>
#include <flakelib/volume/von_neumann/mix_with_center_value.cl>
#include <flakelib/volume/von_neumann/sum.cl>

#define WEIGHTED_JACOBI

kernel void
FLAKELIB_KERNEL_NAME(apply)(
	global float const *FLAKELIB_KERNEL_ARGUMENT(boundary),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	global float const *FLAKELIB_KERNEL_ARGUMENT(rhs),
	global float const *FLAKELIB_KERNEL_ARGUMENT(input),
	global float *FLAKELIB_KERNEL_ARGUMENT(output))
{
	size_t const current_index =
		flakelib_volume_at(
			buffer_pitch,
			flakelib_volume_global_size(),
			flakelib_volume_current_position());

	flakelib_volume_von_neumann_size_t_neighbors neighborhood =
		flakelib_volume_von_neumann_load_neighbor_indices(
			flakelib_volume_current_position(),
			flakelib_volume_global_size(),
			buffer_pitch);

	float const center =
		input[current_index];

	flakelib_volume_von_neumann_float_neighbors floats =
		flakelib_volume_von_neumann_mix_with_center_value(
			&neighborhood,
			boundary,
			input,
			center);

	float const rhs_value =
		rhs[current_index];

	float const result =
		(FLAKELIB_VOLUME_VON_NEUMANN_SUM(floats) - rhs_value) / 6.0f;

	output[current_index] =
#ifndef WEIGHTED_JACOBI
		result;
#else
		1.0f/3.0f * center +
		2.0f/3.0f * result;
#endif
}
