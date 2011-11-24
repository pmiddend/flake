#define LAPLACIAN_RESIDUAL_USES_BOUNDARY

sampler_t const absolute_clamping_nearest =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_NEAREST;

sampler_t const absolute_clamping_linear =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_LINEAR;

constant int2 const
	pos_left = (int2)(-1,0),
	pos_right = (int2)(1,0),
	pos_top = (int2)(0,1),
	pos_bottom = (int2)(0,-1);

kernel void
add(
	global read_only image2d_t p1,
	global read_only image2d_t p2,
	global write_only image2d_t output)
{
	int2 const position =
		(int2)(
			get_global_id(0),
			get_global_id(1));

	write_imagef(
		output,
		position,
		read_imagef(
			p1,
			absolute_clamping_nearest,
			position) +
		read_imagef(
			p2,
			absolute_clamping_nearest,
			position));
}

// from = big
// to = small
// We iterate over the _small_ image and gather texels from the _large_ image.
kernel void
downsample(
	global read_only image2d_t from,
	global write_only image2d_t to)
{
	int2 const position =
		(int2)(
			2 * get_global_id(0),
			2 * get_global_id(1));

	float
		center =
			read_imagef(
				from,
				absolute_clamping_nearest,
				position).x,
		left =
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_left).x,
		right =
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_right).x,
		top =
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom =
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_bottom).x,
		lefttop =
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_top + pos_left).x,
		leftbottom =
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_bottom + pos_left).x,
		rightbottom =
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_bottom + pos_right).x,
		righttop =
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_top + pos_right).x;

	float const
		diagonals =
			leftbottom + lefttop + righttop + rightbottom,
		von_neumann =
			left + right + top + bottom,
		output =
			1.0f/16.0f * (diagonals + 2.0f * von_neumann + 4.0f * center);

	write_imagef(
		to,
		// position is the "bigger" position
		position / 2,
		(float4)(
			output,
			0.0f,
			0.0f,
			0.0f));
}

// from = small
// to = big
// We iterate over the _big_ image.
kernel void
upsample_(
	global read_only image2d_t from,
	global write_only image2d_t to)
{
	int2 const position_in_big_image =
		(int2)(
			get_global_id(0),
			get_global_id(1));

	write_imagef(
		to,
		position_in_big_image,
		read_imagef(
			from,
			absolute_clamping_linear,
			convert_float2(get_image_dim(from)) *
			convert_float2(position_in_big_image) /
			convert_float2(get_image_dim(to)) + (float2)(0.5f,0.5f)));
	/*
	int2 const position =
		(int2)(
			get_global_id(0),
			get_global_id(1));

	if(position.x % 2 == 0 && position.y % 2 == 0)
	{
		write(
			to,
			position,
			read(
				from,
				position/2));
	}
	else if(position.x % 2 == 0)
	{
		float const sum =
			read(from,(int2)(position.x/2,position.y)) +
			read(from,(int2)(position.x/2+1,position.y))

		write(
			to,
			position,
			0.5f * sum);
	}
	else if(position.y % 2 == 0)
	{
		float const sum =
			read(from,(int2)(position.x,position.y/2)) +
			read(from,(int2)(position.x,position.y/2+1))

		write(
			to,
			position,
			0.5f * sum);
	}
	else
	{
		float const sum =
			read(from,(int2)((position.x-1)/2,(position.y-1)/2)) +
			read(from,(int2)((position.x+1)/2,(position.y+1)/2)) +
			read(from,(int2)((position.x-1)/2,(position.y+1)/2)) +
			read(from,(int2)((position.x+1)/2,(position.y-1)/2));

		write(
			to,
			position,
			0.25 * sum);
	}
	*/
}



kernel void
laplacian_residual(
	global read_only image2d_t rhs,
	global read_only image2d_t boundary,
	global read_only image2d_t from,
	global write_only image2d_t to,
	float const grid_scale)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

#ifdef LAPLACIAN_RESIDUAL_USES_BOUNDARY
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
#endif

	float
		center =
			read_imagef(
				from,
				absolute_clamping_nearest,
				position).x,
		left =
#ifdef LAPLACIAN_RESIDUAL_USES_BOUNDARY
			left_boundary *
			center +
			(1.0f - left_boundary) *
#endif
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_left).x,
		right =
#ifdef LAPLACIAN_RESIDUAL_USES_BOUNDARY
			right_boundary *
			center +
			(1.0f - right_boundary) *
#endif
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_right).x,
		top =
#ifdef LAPLACIAN_RESIDUAL_USES_BOUNDARY
			top_boundary *
			center +
			(1.0f - top_boundary) *
#endif
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom =
#ifdef LAPLACIAN_RESIDUAL_USES_BOUNDARY
			bottom_boundary *
			center +
			(1.0f - bottom_boundary) *
#endif
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_bottom).x;

	float const
		laplace =
			(left + right + top + bottom - 4.0f * center) / (grid_scale * grid_scale),
		rhs_value =
			read_imagef(
				rhs,
				absolute_clamping_nearest,
				position).x;

	write_imagef(
		to,
		position,
		(float4)(
			//fabs(laplace - rhs_value),
			rhs_value - laplace,
			0.0f,
			0.0f,
			0.0f));
}
