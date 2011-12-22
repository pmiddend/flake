#include "float_handling.cl"
#include "positions.cl"

sampler_t const absolute_clamping_nearest =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_NEAREST;

kernel void
copy_float_buffer(
	global float const *from,
	global float *to,
	float const multiplier)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	to[get_global_id(0)] =
		multiplier * from[get_global_id(0)];
}

kernel void
null_float_buffer(
	global float *f)
{
	f[get_global_id(0)] =
		0.0f;
}

kernel void
generate_float_oscillation(
	global flake_real *input,
	int const buffer_width)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	flake_real const oscillations =
		FLAKE_REAL_LIT(64.0);

	flake_real const
		sine1 =
			sinpi(
				FLAKE_REAL_LIT(2.0) * oscillations * position.x / buffer_width),
		sine2 =
			sinpi(
				FLAKE_REAL_LIT(2.0) * oscillations * position.y / buffer_width),
		sine3 =
			sinpi(
				FLAKE_REAL_LIT(2.0) * oscillations/FLAKE_REAL_LIT(16.0) * position.x / buffer_width),
		sine4 =
			sinpi(
				FLAKE_REAL_LIT(2.0) * oscillations/FLAKE_REAL_LIT(16.0) * position.y / buffer_width),
		sum =
			clamp(
				sine1 + sine2 + sine3 + sine4,
				FLAKE_REAL_LIT(-1.0),
				FLAKE_REAL_LIT(1.0));

	input[FLAKE_AT(buffer_width,position)] =
		(sum + FLAKE_REAL_LIT(1.0))/FLAKE_REAL_LIT(2.0);
}

kernel void
frobenius_norm_tile(
	global read_only image2d_t input,
	global float *workgroup_results)
{
	int2 const pixels_per_work_unit =
		(int2)(
			(int)ceil((float)get_image_width(input) / (float)get_global_size(0)),
			(int)ceil((float)get_image_height(input) / (float)get_global_size(1)));

	int2 const image_starting_position =
		(int2)(
			get_global_id(0) * pixels_per_work_unit.x,
			get_global_id(1) * pixels_per_work_unit.y);

	float partial_result = 0.0f;

	for(int y = image_starting_position.y; y < image_starting_position.y + pixels_per_work_unit.y; ++y)
	{
		for(int x = image_starting_position.x; x < image_starting_position.x + pixels_per_work_unit.x; ++x)
		{
			if(x >= get_image_width(input) || y >= get_image_height(input))
				continue;

			float const current_value =
				read_imagef(
					input,
					absolute_clamping_nearest,
					(int2)(x,y)).x;

			float const target_value =
				fabs(current_value) * fabs(current_value);

			partial_result += target_value;
		}
	}

	workgroup_results[get_global_id(1) * get_global_size(0) + get_global_id(0)] =
		partial_result;
}

kernel void
frobenius_norm(
	global float *workgroup_results,
	uint const elements,
	global float *total_results)
{
	total_results[0] = 0.0f;
	for(size_t i = 0; i < elements; ++i)
		total_results[0] += workgroup_results[i];
}

kernel void
planar_vector_magnitude(
	global read_only image2d_t input,
	global write_only image2d_t output,
	float const scaling)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	FLAKE_WRITE_IMAGE_FUNCTION(
		output,
		position,
		(flake_real4)(
			length(
				FLAKE_READ_IMAGE_FUNCTION(
					input,
					absolute_clamping_nearest,
					position).xy) * scaling,
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0)));
}
