sampler_t const absolute_clamping_nearest =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_NEAREST;

constant int2 const
	pos_left = (int2)(-1,0),
	pos_right = (int2)(1,0),
	pos_top = (int2)(0,1),
	pos_bottom = (int2)(0,-1);

/**
	Solve the special equation

	Ax=0

	where A is the Laplace operator.
 */
kernel void
jacobi(
	/* 0 */global read_only image2d_t x,
	/* 1 */global write_only image2d_t output,
	/* 2 */float const alpha,
	/* 3 */float const beta)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	float
		center =
			read_imagef(
				x,
				absolute_clamping_nearest,
				position).x,
		left =
			read_imagef(
				x,
				absolute_clamping_nearest,
				position + pos_left).x,
		right =
			read_imagef(
				x,
				absolute_clamping_nearest,
				position + pos_right).x,
		top =
			read_imagef(
				x,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom =
			read_imagef(
				x,
				absolute_clamping_nearest,
				position + pos_bottom).x;

	float const b_value =
		0.0f;

	write_imagef(
		output,
		position,
		(float4)((left + right + top + bottom + alpha * b_value) * beta,0.0f,0.0f,0.0f));
}
