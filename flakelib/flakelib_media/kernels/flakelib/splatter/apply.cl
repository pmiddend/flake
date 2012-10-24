#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>
#include <flakelib/planar/current_position.cl>
#include <flakelib/planar/current_position.cl>
#include <flakelib/planar/global_size.cl>
#include <flakelib/planar/at.cl>
#include <flakelib/volume/current_position.cl>
#include <flakelib/volume/current_position.cl>
#include <flakelib/volume/global_size.cl>
#include <flakelib/volume/at.cl>

kernel void
FLAKELIB_KERNEL_NAME(splat_planar_float)(
	global float *FLAKELIB_KERNEL_ARGUMENT(input),
	int const FLAKELIB_KERNEL_ARGUMENT(pen_position_x),
	int const FLAKELIB_KERNEL_ARGUMENT(pen_position_y),
	int const FLAKELIB_KERNEL_ARGUMENT(buffer_width),
	int const FLAKELIB_KERNEL_ARGUMENT(buffer_height),
	int const FLAKELIB_KERNEL_ARGUMENT(is_rectangle),
	int const FLAKELIB_KERNEL_ARGUMENT(is_hard),
	int const FLAKELIB_KERNEL_ARGUMENT(do_mixing),
	float const FLAKELIB_KERNEL_ARGUMENT(blend_factor),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	float const FLAKELIB_KERNEL_ARGUMENT(value))
{
	int2 const
		current_internal_position =
			flakelib_planar_current_position(),
		pen_position =
			(int2)(
				pen_position_x,
				pen_position_y),
		current_position =
			pen_position + current_internal_position,
		pen_radiuses =
			flakelib_planar_global_size() / 2,
		pen_radiuses_squared =
			pen_radiuses * pen_radiuses,
		pen_center =
			pen_position + pen_radiuses,
		position_to_test =
			current_position - pen_center;

	if(current_position.x >= buffer_width || current_position.x < 0 || current_position.y >= buffer_height || current_position.y < 0)
		return;

	float2 const
		ellipse_components =
			convert_float2(position_to_test * position_to_test) / convert_float2(pen_radiuses_squared);

	float multiplier =
		1.0f -
		clamp(
			ellipse_components.x + ellipse_components.y,
			0.0f,
			1.0f);

	if(is_rectangle)
	{
		// There are only hard rectangles
		multiplier =
			all(
				current_position >= pen_position &&
				current_position <= pen_position + pen_radiuses*2)
			?
				1.0f
			:
				0.0f;
	}
	else
	{
		if(is_hard)
			multiplier =
				step(
					0.01f,
					multiplier);
	}


	float const old_value =
		input[flakelib_planar_at(buffer_pitch,current_position)];

	if(do_mixing)
	{
		input[flakelib_planar_at(buffer_pitch,current_position)] =
			mix(
				old_value,
				multiplier * value,
				blend_factor);
	}
	else
	{
		input[flakelib_planar_at(buffer_pitch,current_position)] =
			old_value +
			blend_factor * multiplier * value;
	}
}

kernel void
FLAKELIB_KERNEL_NAME(splat_planar_float2)(
	global float2 *FLAKELIB_KERNEL_ARGUMENT(input))
{
}

kernel void
FLAKELIB_KERNEL_NAME(splat_volume_float)(
	global float *FLAKELIB_KERNEL_ARGUMENT(input),
	int const FLAKELIB_KERNEL_ARGUMENT(pen_position_x),
	int const FLAKELIB_KERNEL_ARGUMENT(pen_position_y),
	int const FLAKELIB_KERNEL_ARGUMENT(pen_position_z),
	int const FLAKELIB_KERNEL_ARGUMENT(buffer_width),
	int const FLAKELIB_KERNEL_ARGUMENT(buffer_height),
	int const FLAKELIB_KERNEL_ARGUMENT(buffer_depth),
	int const FLAKELIB_KERNEL_ARGUMENT(is_box),
	int const FLAKELIB_KERNEL_ARGUMENT(is_hard),
	int const FLAKELIB_KERNEL_ARGUMENT(do_mixing),
	float const FLAKELIB_KERNEL_ARGUMENT(blend_factor),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	float const FLAKELIB_KERNEL_ARGUMENT(value))
{
	int4 const
		current_internal_position =
			flakelib_volume_current_position(),
		pen_position =
			(int4)(
				pen_position_x,
				pen_position_y,
				pen_position_z,
				0),
		current_position =
			pen_position + current_internal_position,
		pen_radiuses =
			flakelib_volume_global_size() / 2,
		pen_radiuses_squared =
			pen_radiuses * pen_radiuses,
		pen_center =
			pen_position + pen_radiuses,
		position_to_test =
			current_position - pen_center;

	if(current_position.x >= buffer_width || current_position.x < 0 || current_position.y >= buffer_height || current_position.y < 0 || current_position.z >= buffer_depth || current_position.z < 0)
		return;

	// This is a division by zero in the w component. Hope nobody minds.
	float4 const
		ellipse_components =
			convert_float4(position_to_test * position_to_test) / convert_float4(pen_radiuses_squared);

	float multiplier =
		1.0f -
		clamp(
			ellipse_components.x + ellipse_components.y + ellipse_components.z,
			0.0f,
			1.0f);

	if(is_box)
	{
		// There are only hard rectangles
		multiplier =
			all(
				current_position >= pen_position &&
				current_position <= pen_position + pen_radiuses*2)
			?
				1.0f
			:
				0.0f;
	}
	else
	{
		if(is_hard)
			multiplier =
				step(
					0.01f,
					multiplier);
	}

	size_t current_position_index =
		flakelib_volume_at(
			buffer_pitch,
			(int4)(
				buffer_width,
				buffer_height,
				buffer_depth,
				0),
			current_position);

	float const old_value =
		input[current_position_index];

	if(do_mixing)
	{
		input[current_position_index] =
			mix(
				old_value,
				multiplier * value,
				blend_factor);
	}
	else
	{
		input[current_position_index] =
			old_value +
			blend_factor * multiplier * value;
	}
}
