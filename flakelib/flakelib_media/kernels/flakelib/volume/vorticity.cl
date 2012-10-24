#include <flakelib/kernel_name.cl>
#include <flakelib/volume/global_size.cl>
#include <flakelib/volume/at.cl>
#include <flakelib/volume/left_of.cl>
#include <flakelib/volume/right_of.cl>
#include <flakelib/volume/top_of.cl>
#include <flakelib/volume/bottom_of.cl>
#include <flakelib/kernel_argument.cl>
#include <flakelib/volume/current_position.cl>
#include <flakelib/volume/von_neumann/float4_neighbors.cl>
#include <flakelib/volume/von_neumann/load_neighbors.cl>
#include <flakelib/volume/von_neumann/apply_binary_operation.cl>
#include <flakelib/volume/von_neumann/float4_gradient.cl>

kernel void
FLAKELIB_KERNEL_NAME(vorticity_apply)(
	global float const *FLAKELIB_KERNEL_ARGUMENT(boundary),
	global float4 const *FLAKELIB_KERNEL_ARGUMENT(velocity),
	global float4 *FLAKELIB_KERNEL_ARGUMENT(output),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch))
{
	int4 const position =
		flakelib_volume_current_position();

	size_t const current_index =
		flakelib_volume_at(
			buffer_pitch,
			flakelib_volume_global_size(),
			position);

	if(position.x <= 2 || position.y <= 2 || position.z <= 2 || position.x >= buffer_pitch-3 || position.y >= buffer_pitch-3 || position.z >= buffer_pitch-3)
	{
		output[current_index] =
			0.0f;
		return;
	}

	flakelib_volume_von_neumann_size_t_neighbors neighborhood =
		flakelib_volume_von_neumann_load_neighbor_indices(
			flakelib_volume_current_position(),
			flakelib_volume_global_size(),
			buffer_pitch);

	// The mix here hopefully enforces that the boundaries are ignored. The
	// velocity at the boundary is set to zero, which under normal
	// circumstances contributes to the vorticity.
	float4
		left =
			mix(
				velocity[neighborhood.left],
				velocity[neighborhood.right],
				boundary[neighborhood.left]),
		right =
			mix(
				velocity[neighborhood.right],
				velocity[neighborhood.left],
				boundary[neighborhood.right]),
		top =
			mix(
				velocity[neighborhood.top],
				velocity[neighborhood.bottom],
				boundary[neighborhood.top]),
		bottom =
			mix(
				velocity[neighborhood.bottom],
				velocity[neighborhood.top],
				boundary[neighborhood.bottom]),
		front =
			mix(
				velocity[neighborhood.front],
				velocity[neighborhood.back],
				boundary[neighborhood.front]),
		back =
			mix(
				velocity[neighborhood.back],
				velocity[neighborhood.front],
				boundary[neighborhood.back]);

	output[current_index] =
		(float4)(
			(bottom.z - top.z) - (back.y - front.y),
			(right.z - left.z) - (back.x - front.x),
			(right.y - left.y) - (bottom.x - top.x),
			0.0f);
}

kernel void
FLAKELIB_KERNEL_NAME(vorticity_gradient_and_cross)(
	float const FLAKELIB_KERNEL_ARGUMENT(vorticity_strength),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	global float const *FLAKELIB_KERNEL_ARGUMENT(vorticity),
	global float4 const *FLAKELIB_KERNEL_ARGUMENT(velocity),
	global float4 *FLAKELIB_KERNEL_ARGUMENT(output),
	float const FLAKELIB_KERNEL_ARGUMENT(dt))
{
	int4 const position =
		flakelib_volume_current_position();

	size_t const current_index =
		flakelib_volume_at(
			buffer_pitch,
			flakelib_volume_global_size(),
			position);

	if(position.x <= 2 || position.y <= 2 || position.z <= 2 || position.x >= buffer_pitch-3 || position.y >= buffer_pitch-3 || position.z >= buffer_pitch-3)
	{
		output[current_index] =
			velocity[current_index];
		return;
	}

#define FLAKELIB_VOLUME_VORTICITY_FABS_OPERATION(source,target)\
	target = fabs(source)

	flakelib_volume_von_neumann_float4_neighbors floats;

	FLAKELIB_VOLUME_VON_NEUMANN_LOAD_NEIGHBORS(
		vorticity,
		floats,
		position,
		flakelib_volume_global_size(),
		buffer_pitch);

	FLAKELIB_VOLUME_VON_NEUMANN_APPLY_BINARY_OPERATION(
		FLAKELIB_VOLUME_VORTICITY_FABS_OPERATION,
		floats,
		floats);

	float4 const gradient =
		flakelib_volume_von_neumann_float4_gradient(
			&floats);

	float const gradient_magnitude =
		fast_length(gradient);

	// I _think_ this guard against small gradient magnitude makes sense.
	// Otherwise, we have a division by zero (or something like that) and have
	// a gradient which simply doesn't exist (it should be zero).
	float4 const normalized_gradient =
		gradient_magnitude < 0.01f
		?
			(float4)(0.0f)
		:
			gradient / gradient_magnitude;

	output[current_index] =
		velocity[current_index] +
		dt *
		vorticity_strength *
		min(
			(float4)(1.0f,1.0f,1.0f,1.0f),
			cross(
				normalized_gradient,
				vorticity[current_index]));
}
