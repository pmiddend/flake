#include "planar/positions.cl"

kernel void
vorticity(
	global float const *boundary,
	global float2 const *velocity,
	global float *output,
	int const buffer_width)
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

	float2
		left =
			mix(
				velocity[left_index],
				(float2)(0.0f),
				boundary[left_index]),
		right =
			mix(
				velocity[right_index],
				(float2)(0.0f),
				boundary[right_index]),
		top =
			mix(
				velocity[top_index],
				(float2)(0.0f),
				boundary[top_index]),
		bottom =
			mix(
				velocity[bottom_index],
				(float2)(0.0f),
				boundary[bottom_index]);

	output[current_index] =
//		0.5f * ((right.y - left.y) - (top.x - bottom.x));
		0.5f * ((right.y - left.y) - (bottom.x - top.x));
}

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
