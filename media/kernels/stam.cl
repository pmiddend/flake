sampler_t const absolute_clamping_nearest =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_NEAREST;

__kernel void
reset_vector_field(
	__global __write_only image2d_t f)
{
	write_imagef(
		f,
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1)),
		(float4)(
			0.5f,
			0.0f,
			0.0f,
			0.0f));
}

__kernel void
copy_boundary(
	__global __write_only image2d_t vector_field,
	__global __read_only image2d_t boundary)
{
	int2 position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	if(read_imagei(boundary,absolute_clamping_nearest,position).x != 0)
		write_imagef(
			vector_field,
			position,
			(float4)(0.0f,0.0f,0.0f,0.0f));
}
