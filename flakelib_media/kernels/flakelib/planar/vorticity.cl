#include <flakelib/kernel_name.cl>
#include <flakelib/planar/global_size.cl>
#include <flakelib/planar/at.cl>
#include <flakelib/planar/left_of.cl>
#include <flakelib/planar/right_of.cl>
#include <flakelib/planar/top_of.cl>
#include <flakelib/planar/bottom_of.cl>
#include <flakelib/kernel_argument.cl>
#include <flakelib/planar/current_position.cl>

kernel void
FLAKELIB_KERNEL_NAME(apply_vorticity)(
	global float const *FLAKELIB_KERNEL_ARGUMENT(boundary),
	global float2 const *FLAKELIB_KERNEL_ARGUMENT(velocity),
	global float *FLAKELIB_KERNEL_ARGUMENT(output),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch))
{
	int2 const position =
		flakelib_planar_current_position();

	int const
		current_index =
			flakelib_planar_at(
				buffer_pitch,
				position),
		left_index =
			flakelib_planar_left_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		right_index =
			flakelib_planar_right_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		top_index =
			flakelib_planar_top_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		bottom_index =
			flakelib_planar_bottom_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position);

	float2
		left =
			(1.0f - boundary[left_index]) * velocity[left_index],
		right =
			(1.0f - boundary[right_index]) * velocity[right_index],
		top =
			(1.0f - boundary[top_index]) * velocity[top_index],
		bottom =
			(1.0f - boundary[bottom_index]) * velocity[bottom_index];

	output[current_index] =
//		0.5f * ((right.y - left.y) - (top.x - bottom.x));
		0.5f * ((right.y - left.y) - (bottom.x - top.x));
}

kernel void
FLAKELIB_KERNEL_NAME(gradient_and_cross)(
	float const FLAKELIB_KERNEL_ARGUMENT(vorticity_strength),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	global float const *FLAKELIB_KERNEL_ARGUMENT(vorticity),
	global float2 *FLAKELIB_KERNEL_ARGUMENT(output),
	float const FLAKELIB_KERNEL_ARGUMENT(dt))
{
	int2 const position =
		flakelib_planar_current_position();

	int const
		current_index =
			flakelib_planar_at(
				buffer_pitch,
				position),
		left_index =
			flakelib_planar_left_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		right_index =
			flakelib_planar_right_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		top_index =
			flakelib_planar_top_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		bottom_index =
			flakelib_planar_bottom_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position);

	float const
		left =
			fabs(vorticity[left_index]),
		right =
			fabs(vorticity[right_index]),
		top =
			fabs(vorticity[top_index]),
		bottom =
			fabs(vorticity[bottom_index]);

	float2 const gradient =
		0.5f *
		(float2)(
			right-left,
			bottom-top);

	// TODO: What about a zero vector here?
	float2 const normalized_gradient =
		fast_normalize(
			gradient);

	output[current_index] =
		normalized_gradient;
	/*
	output[current_index] +=
		dt *
		strength *
		normalized_gradient.yx;
		*/
}
