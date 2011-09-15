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
			1.0f,
			0.5f,
			0.0f,
			0.0f));
}
