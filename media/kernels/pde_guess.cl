kernel void generate_guess(
	global write_only image2d_t input)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	float const oscillations =
		8.0f;

	write_imagef(
		input,
		position,
		(float4)(
			(sinpi(2.0f * oscillations * position.y / get_image_width(input)) + 1.0f)/2.0f,
			1.0f,
			1.0f,
			1.0f));
}
