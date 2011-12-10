#include "float_handling.cl"

sampler_t const absolute_clamping_nearest =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_NEAREST;

kernel void
copy_image(
	global read_only image2d_t from,
	global write_only image2d_t to,
	float const multiplier)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	FLAKE_WRITE_IMAGE_FUNCTION(
		to,
		position,
		FLAKE_FROM_FLOAT(multiplier) *
		FLAKE_READ_IMAGE_FUNCTION(
			from,
			absolute_clamping_nearest,
			position));
}

kernel void
null_image(
	global write_only image2d_t f)
{
	FLAKE_WRITE_IMAGE_FUNCTION(
		f,
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1)),
		(flake_real4)(
			FLAKE_REAL_LIT(0.0)));
}

kernel void
generate_oscillation(
	global write_only image2d_t input)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	flake_real const oscillations =
		FLAKE_REAL_LIT(32.0);

	flake_real const
		sine1 =
			sinpi(FLAKE_REAL_LIT(2.0) * oscillations * position.x / get_image_width(input)),
		sine2 =
			sinpi(FLAKE_REAL_LIT(2.0) * oscillations * position.y / get_image_height(input)),
		sine3 =
			sinpi(FLAKE_REAL_LIT(2.0) * FLAKE_REAL_LIT(1.0)/FLAKE_REAL_LIT(4.0) * oscillations * position.x / get_image_width(input)),
		sine4 =
			sinpi(FLAKE_REAL_LIT(2.0) * FLAKE_REAL_LIT(1.0)/FLAKE_REAL_LIT(4.0) * oscillations * position.y / get_image_height(input)),
		sum = clamp(sine1 + sine2 + sine3 + sine4,FLAKE_REAL_LIT(-1.0),FLAKE_REAL_LIT(1.0));

	FLAKE_WRITE_IMAGE_FUNCTION(
		input,
		position,
		(flake_real4)(
			(sum + FLAKE_REAL_LIT(1.0))/FLAKE_REAL_LIT(2.0),
			FLAKE_REAL_LIT(1.0),
			FLAKE_REAL_LIT(1.0),
			FLAKE_REAL_LIT(1.0)));
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
