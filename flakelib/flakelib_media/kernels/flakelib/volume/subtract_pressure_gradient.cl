#include <flakelib/volume/current_position.cl>
#include <flakelib/volume/global_size.cl>
#include <flakelib/volume/at.cl>
#include <flakelib/kernel_argument.cl>
#include <flakelib/kernel_name.cl>
#include <flakelib/boundary_is_solid.cl>
#include <flakelib/volume/von_neumann/size_t_neighbors.cl>
#include <flakelib/volume/von_neumann/load_neighbor_indices.cl>
#include <flakelib/volume/von_neumann/mix_with_center_value.cl>
#include <flakelib/volume/von_neumann/float_gradient.cl>

kernel void
FLAKELIB_KERNEL_NAME(subtract_pressure_gradient_apply)(
	global float const *FLAKELIB_KERNEL_ARGUMENT(pressure),
	global float4 *FLAKELIB_KERNEL_ARGUMENT(velocity),
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

	if(flakelib_boundary_is_solid(boundary,current_index))
	{
		velocity[current_index] =
			(float4)(
				0.0f,
				0.0f,
				0.0f,
				0.0f);
		return;
	}

	float const center =
		pressure[current_index];

	flakelib_volume_von_neumann_float_neighbors pressure_neighbors =
		flakelib_volume_von_neumann_mix_with_center_value(
			&neighborhood,
			boundary,
			pressure,
			center);

	float4 const vmask =
		(float4)(
			1.0f -
			min(
				1.0f,
				boundary[neighborhood.left] + boundary[neighborhood.right]),
			1.0f -
			min(
				1.0f,
				boundary[neighborhood.top] + boundary[neighborhood.bottom]),
			1.0f -
			min(
				1.0f,
				boundary[neighborhood.back] + boundary[neighborhood.front]),
			0.0f);

	float4 const pressure_gradient =
		flakelib_volume_von_neumann_float_gradient(
			&pressure_neighbors);

	velocity[current_index] =
		vmask * (velocity[current_index] - pressure_gradient);
}
