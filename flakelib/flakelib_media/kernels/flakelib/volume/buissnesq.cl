#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>

kernel void
FLAKELIB_KERNEL_NAME(buissnesq_apply)(
	float const FLAKELIB_KERNEL_ARGUMENT(temperature_strength),
	float const FLAKELIB_KERNEL_ARGUMENT(density_strength),
	float const FLAKELIB_KERNEL_ARGUMENT(ambient_temperature),
	float const FLAKELIB_KERNEL_ARGUMENT(time_delta),
	global float4 *FLAKELIB_KERNEL_ARGUMENT(velocity),
	global float const *FLAKELIB_KERNEL_ARGUMENT(temperature),
	global float const *FLAKELIB_KERNEL_ARGUMENT(density))
{
	velocity[get_global_id(0)] +=
		(float4)(
			0.0f,
			time_delta *
			(
				density_strength * density[get_global_id(0)] -
				temperature_strength * (temperature[get_global_id(0)] - ambient_temperature)),
			0.0f,
			0.0f);
}
