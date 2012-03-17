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

#if 0
kernel void
gradient_and_cross(
	float const strength,
	int const buffer_width,
	global float const *curl,
	global float2 *output,
	float const dt)
{
	int2 const currentpos =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	int const
		current_index =
			FLAKE_PLANAR_AT(buffer_width,currentpos),
		left_index =
			FLAKE_PLANAR_LEFT_OF(buffer_width,currentpos),
		right_index =
			FLAKE_PLANAR_RIGHT_OF(buffer_width,currentpos),
		top_index =
			FLAKE_PLANAR_TOP_OF(buffer_width,currentpos),
		bottom_index =
			FLAKE_PLANAR_BOTTOM_OF(buffer_width,currentpos);

	float
		center =
			fabs(curl[current_index]),
		left =
			fabs(curl[left_index]),
		right =
			fabs(curl[right_index]),
		top =
			fabs(curl[top_index]),
		bottom =
			fabs(curl[bottom_index]);

	/*
	// Use mix here? What about the vmask?
	if(is_solid(boundary,left_index))
		left = center;

	if(is_solid(boundary,right_index))
		right = center;

	if(is_solid(boundary,top_index))
		top = center;

	if(is_solid(boundary,bottom_index))
		bottom = center;
		*/

	float2 const gradient =
		0.5f *
		(float2)(
			right-left,
			bottom-top);

	float2 const normalized_gradient =
		gradient /
		max(
			0.0001f,
			fast_length(
				gradient));

	output[current_index] +=
		dt *
		strength *
		normalized_gradient.yx;
}
#endif
