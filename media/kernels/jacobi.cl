#define WEIGHTED_JACOBI

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
	/* 0 */float const alpha,
	/* 1 */float const beta,
	/* 2 */global read_only image2d_t rhs,
	/* 3 */global read_only image2d_t boundary,
	/* 4 */global read_only image2d_t x,
	/* 5 */global write_only image2d_t output)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	float const
		left_boundary =
			read_imagef(
				boundary,
				absolute_clamping_nearest,
				position + pos_left).x,
		right_boundary =
			read_imagef(
				boundary,
				absolute_clamping_nearest,
				position + pos_right).x,
		top_boundary =
			read_imagef(
				boundary,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom_boundary =
			read_imagef(
				boundary,
				absolute_clamping_nearest,
				position + pos_bottom).x;

	float
		center =
			read_imagef(
				x,
				absolute_clamping_nearest,
				position).x,
		left =
			left_boundary *
			center +
			(1.0f - left_boundary) *
			read_imagef(
				x,
				absolute_clamping_nearest,
				position + pos_left).x,
		right =
			right_boundary *
			center +
			(1.0f - right_boundary) *
			read_imagef(
				x,
				absolute_clamping_nearest,
				position + pos_right).x,
		top =
			top_boundary *
			center +
			(1.0f - top_boundary) *
			read_imagef(
				x,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom =
			bottom_boundary *
			center +
			(1.0f - bottom_boundary) *
			read_imagef(
				x,
				absolute_clamping_nearest,
				position + pos_bottom).x;

	float const b_value =
		read_imagef(
				rhs,
				absolute_clamping_nearest,
				position).x;

	write_imagef(
		output,
		position,
//#ifndef WEIGHTED_JACOBI
		(float4)((left + right + top + bottom + alpha * b_value) * beta,0.0f,0.0f,0.0f));
//#else
//		(float4)(1.0f/3.0f * center + 2.0f / 3.0f * (left + right + top + bottom + alpha * b_value) * beta,0.0f,0.0f,0.0f));
//#endif
}
