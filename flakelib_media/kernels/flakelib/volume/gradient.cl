#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>
#include <flakelib/volume/global_size.cl>
#include <flakelib/volume/current_position.cl>
#include <flakelib/volume/von_neumann/float_neighbors.cl>
#include <flakelib/volume/von_neumann/gradient.cl>
#include <flakelib/volume/von_neumann/load_neighbors.cl>

kernel void
FLAKELIB_KERNEL_NAME(gradient_apply)(
	global float const *FLAKELIB_KERNEL_ARGUMENT(input),
	global float4 *FLAKELIB_KERNEL_ARGUMENT(output),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch))
{
	flakelib_volume_von_neumann_float_neighbors float_neighbors;

	FLAKELIB_VOLUME_VON_NEUMANN_LOAD_NEIGHBORS(
		input,
		float_neighbors,
		flakelib_volume_current_position(),
		flakelib_volume_global_size(),
		buffer_pitch)

	output[
		flakelib_volume_at(
			buffer_pitch,
			flakelib_volume_global_size(),
			flakelib_volume_current_position())] =
		flakelib_volume_von_neumann_gradient(
			&float_neighbors);
}
