#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>
#include <flakelib/boundary_is_solid.cl>
#include <flakelib/volume/global_size.cl>
#include <flakelib/volume/current_position.cl>
#include <flakelib/volume/at.cl>
#include <flakelib/volume/von_neumann/load_neighbor_indices.cl>
#include <flakelib/volume/von_neumann/float4_neighbors.cl>
#include <flakelib/volume/von_neumann/apply_binary_operation.cl>
#include <flakelib/volume/von_neumann/divergence.cl>

kernel void
FLAKELIB_KERNEL_NAME(divergence_apply)(
	global float4 const *FLAKELIB_KERNEL_ARGUMENT(input),
	global float *FLAKELIB_KERNEL_ARGUMENT(output),
	global float const *FLAKELIB_KERNEL_ARGUMENT(boundary),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch))
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

	flakelib_volume_von_neumann_float4_neighbors floats;

#define FLAKELIB_VOLUME_DIVERGENCE_MIX_OPERATION(source,target)\
	target = (1.0f - boundary[source]) * input[source]

	FLAKELIB_VOLUME_VON_NEUMANN_APPLY_BINARY_OPERATION(
		FLAKELIB_VOLUME_DIVERGENCE_MIX_OPERATION,
		neighborhood,
		floats);

	output[
		current_index] =
		flakelib_volume_von_neumann_divergence(
			&floats);
}
