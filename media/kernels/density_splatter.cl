sampler_t const absolute_clamping_nearest =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_NEAREST;

kernel void
splat(
	global write_only image2d_t output,
	int const start_x,
	int const start_y)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	write_imagef(
		output,
		(int2)(start_x,start_y) + position,
		(float4)(
			1.0f,
			1.0f,
			1.0f,
			1.0f));
}
