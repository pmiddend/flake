sampler_t const absolute_clamping_nearest =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_NEAREST;

kernel void
copy_image(
	global read_only image2d_t from,
	global write_only image2d_t to)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));
	write_imagef(
		to,
		position,
		read_imagef(
			from,
			absolute_clamping_nearest,
			position));
}

kernel void
null_image(
	global write_only image2d_t f)
{
	write_imagef(
		f,
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1)),
		(float4)(
			0.0f,
			0.0f,
			0.0f,
			0.0f));
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

	float const oscillations =
		32.0f;

	write_imagef(
		input,
		position,
		(float4)(
			(sinpi(2.0f * oscillations * position.y / get_image_width(input)) + 1.0f)/2.0f,
			1.0f,
			1.0f,
			1.0f));
}
