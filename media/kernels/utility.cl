#include "planar/positions.cl"

kernel void
copy_float_buffer(
	global float const *from,
	global float *to,
	float const multiplier)
{
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
	global float *input,
	int const buffer_width)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	float const oscillations =
		64.0f;

	float const
		sine1 =
			sinpi(
				2.0f * oscillations * position.x / buffer_width),
		sine2 =
			sinpi(
				2.0f * oscillations * position.y / buffer_width),
		sine3 =
			sinpi(
				2.0f * oscillations/16.0f * position.x / buffer_width),
		sine4 =
			sinpi(
				2.0f * oscillations/16.0f * position.y / buffer_width),
		sum =
			clamp(
				sine1 + sine2 + sine3 + sine4,
				-1.0f,
				1.0f);

	input[FLAKE_PLANAR_AT(buffer_width,position)] =
		(sum + 1.0f)/2.0f;
}

kernel void
frobenius_norm_tile(
	global float const *input,
	global float *workgroup_results,
	int const input_size,
	int const pixels_to_process)
{
	float partial_result = 0.0f;

	for(
		size_t
			x =
				(size_t)(pixels_to_process * get_global_id(0)),
			end =
				min((size_t)(input_size-1), (size_t)(x + pixels_to_process));
		x < end;
		++x)
	{
		float const current_value =
			input[x];

		float const target_value =
			fabs(current_value) * fabs(current_value);

		partial_result += target_value;
	}

	workgroup_results[get_global_id(0)] =
		partial_result;
}

kernel void
planar_vector_magnitude(
	global float2 const *input,
	global float *output,
	int const buffer_width,
	float const scaling)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	output[position.y * buffer_width + position.x] =
		scaling *
		length(
			input[position.y * buffer_width + position.x]);
}
