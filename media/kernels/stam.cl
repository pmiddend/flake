// With non-normalized coordinates, only CLAMP and CLAMP_TO_EDGE are supported.
// CLAMP will clamp to a border color.
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

bool
is_solid(
	global read_only image2d_t boundary,
	int2 const position)
{
	return
		read_imagei(
			boundary,
			absolute_clamping_nearest,position).x != 0;
}

kernel void
advect(
	global read_only image2d_t input,
	global write_only image2d_t output,
	global read_only image2d_t boundary,
	float const dt,
	float const grid_scale)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	if(is_solid(boundary,position))
	{
		write_imagef(
			output,
			position,
			(float4)(0.0f));
		return;
	}

	// Retrieve the (imagined) particle's current velocity. We can
	// get this using a nearest filter, since we're at an exact
	// pixel location (given by the kernel's global id)
	float2 const current_velocity =
		read_imagef(
			input,
			absolute_clamping_nearest,
			position).xy;

	// From the current position, assume the particle moves "back
	// in time". Use its current velocity and move it by -dt.
	float2 const advected_vector =
		convert_float2(position) -
		dt *
		(1.0f / grid_scale) *
		current_velocity;

	// After moving back in time, we do not always end up exactly at a grid
	// cell's center, we might be inbetween grid cells. Luckily, we can
	// interpolate.
	//
	// Notice the linear interpolation here. If we use CLAMP instead of
	// CLAMP_TO_EDGE, we clamp to a border color, which is the equivalent of
	// black.
	//
	// Also note that this is the place where "diffusion" occurs. At cells with
	// zero velocity, the only thing we do is take the current position as an 
	// integer, convert it to float and subtract (0,0) from it. Then we write it back.
	// However, the conversion to float loses information and we end up with a
	// slighly off-grid position. Together with linear interpolation, this leads to
	// diffusion.
	//
	// Also note that using CLAMP_TO_EDGE leads to border vectors on the left
	// that point to the right (zero y coordinate) to not be changed.
	float4 const interpolated_vector =
		read_imagef(
			input,
			absolute_clamping_linear,
			// Linear interpolation uses (u,v) - (0.5,0.5) as "reference", so
			// compensate for that using an addition here.
			advected_vector + (float2)(0.5f,0.5f));

	write_imagef(
		output,
		position,
		interpolated_vector);
}

/*
This function iterates over all the left border pixels of the image and assigns
a fixed force vector to them. This vector points to the right and has a given
magnitude.
*/
kernel void
apply_external_forces(
	/* 0 */global read_only image2d_t input,
	/* 1 */global write_only image2d_t output,
	/* 2 */float const force_magnitude,
	/* 3 */unsigned const fan_width,
	/* 4 */float const grid_scale,
	/* 5 */float const dt)
{
	size_t const i =
		get_global_id(
			0);

	unsigned const edge_size =
		get_image_width(
			input);

	/*
	write_imagef(
		output,
		(int2)(0,i),
		read_imagef(
			input,
			absolute_clamping_nearest,
			(int2)(1,i)));
		*/
	write_imagef(
		output,
		(int2)(0,i),
		(float4)(
			force_magnitude,
			0.0f,
			0.0f,
			0.0f));

	write_imagef(
		output,
		(int2)(i,0),
		read_imagef(
			input,
			absolute_clamping_nearest,
			(int2)(i,1)));

	write_imagef(
		output,
		(int2)(edge_size-1,i),
		read_imagef(
			input,
			absolute_clamping_nearest,
			(int2)(edge_size-2,i)));

	write_imagef(
		output,
		(int2)(i,edge_size-1),
		read_imagef(
			input,
			absolute_clamping_nearest,
			(int2)(i,edge_size-2)));

	/*
	if(i >= start_y && i < start_y + 2*fan_width)
	{
		write_imagef(
			output,
			(int2)(0,i),
			(float4)(
				fast_normalize(
					(float2)(
						force_magnitude,
						i - middle)),
				0.0f,
				0.0f));
	}*/
}

kernel void
divergence(
	global read_only image2d_t input,
	global write_only image2d_t output,
	global read_only image2d_t boundary,
	float const grid_scale)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	float2
		left =
			read_imagef(
				input,
				absolute_clamping_nearest,
				position + pos_left).xy,
		right =
			read_imagef(
				input,
				absolute_clamping_nearest,
				position + pos_right).xy,
		top =
			read_imagef(
				input,
				absolute_clamping_nearest,
				position + pos_top).xy,
		bottom =
			read_imagef(
				input,
				absolute_clamping_nearest,
				position + pos_bottom).xy;

	if(is_solid(boundary,position + pos_left))
		left = (float2)(0.0f,0.0f);
	if(is_solid(boundary,position + pos_right))
		right = (float2)(0.0f,0.0f);
	if(is_solid(boundary,position + pos_top))
		top = (float2)(0.0f,0.0f);
	if(is_solid(boundary,position + pos_bottom))
		bottom = (float2)(0.0f,0.0f);

	write_imagef(
		output,
		position,
		(float4)(
			((right.x - left.x) + (top.y - bottom.y)) / (2.0f * grid_scale),
			0.0f,
			0.0f,
			0.0f));
}

// Solves Ax=b for x, where A is the Laplace Operator.
// Explained (in German):
// http://mo.mathematik.uni-stuttgart.de/kurse/kurs5/seite36.html
// Convergence: Extremely slow
kernel void
jacobi(
	/* 0 */global read_only image2d_t b,
	/* 1 */global read_only image2d_t x,
	/* 2 */global read_only image2d_t boundary,
	/* 3 */global write_only image2d_t output,
	/* 4 */float const alpha,
	/* 5 */float const beta)
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

	/*
	if(is_solid(boundary,position + pos_left))
		left = center;
	if(is_solid(boundary,position + pos_right))
		right = center;
	if(is_solid(boundary,position + pos_top))
		top = center;
	if(is_solid(boundary,position + pos_bottom))
		bottom = center;
		*/

	float const b_value =
		read_imagef(
			b,
			absolute_clamping_nearest,
			position).x;

	write_imagef(
		output,
		position,
		(float4)((left + right + top + bottom + alpha * b_value) * beta,0.0f,0.0f,0.0f));
}

// Calculate the gradient of p and calculate
// w - gradient(p)
kernel void
gradient_and_subtract(
	/* 0 */global read_only image2d_t p,
	/* 1 */float const grid_scale,
	/* 2 */global read_only image2d_t w,
	/* 3 */global read_only image2d_t boundary,
	/* 4 */global write_only image2d_t output)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	if(is_solid(boundary,position))
	{
		write_imagef(
			output,
			position,
			(float4)(0.0f,0.0f,0.0f,0.0f));
		return;
	}

	float
		center =
			read_imagef(
				p,
				absolute_clamping_nearest,
				position).x,
		left =
			read_imagef(
				p,
				absolute_clamping_nearest,
				position + pos_left).x,
		right =
			read_imagef(
				p,
				absolute_clamping_nearest,
				position + pos_right).x,
		top =
			read_imagef(
				p,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom =
			read_imagef(
				p,
				absolute_clamping_nearest,
				position + pos_bottom).x;

	float2 vmask =
		(float2)(1.0f,1.0f);

	if(is_solid(boundary,position + pos_left))
	{
		left = center;
		vmask.x = 0.0f;
	}

	if(is_solid(boundary,position + pos_right))
	{
		right = center;
		vmask.x = 0.0f;
	}

	if(is_solid(boundary,position + pos_top))
	{
		top = center;
		vmask.y = 0.0f;
	}

	if(is_solid(boundary,position + pos_bottom))
	{
		bottom = center;
		vmask.y = 0.0f;
	}

	float2 const velocity =
		read_imagef(
			w,
			absolute_clamping_nearest,
			position).xy;

	float2 const pressure_gradient =
		(0.5f / grid_scale) *
			(float2)(
				right-left,
				top-bottom);

	float2 result =
		velocity - pressure_gradient;

	result = vmask * result;

	write_imagef(
		output,
		position,
		(float4)(result.xy,0.0f,0.0f));
}

kernel void
laplacian_residual_absolute_value(
	/* 0 */global read_only image2d_t rhs,
	/* 1 */global read_only image2d_t boundary,
	/* 2 */global read_only image2d_t from,
	/* 3 */global write_only image2d_t to,
	/* 4 */float const grid_scale)
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
				from,
				absolute_clamping_nearest,
				position).x,
		left =
			left_boundary *
			center +
			(1.0f - left_boundary) *
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_left).x,
		right =
			right_boundary *
			center +
			(1.0f - right_boundary) *
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_right).x,
		top =
			top_boundary *
			center +
			(1.0f - top_boundary) *
			read_imagef(
				from,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom =
			bottom_boundary *
			center +
			(1.0f - bottom_boundary) *
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
			laplace - rhs_value,
			0.0f,
			0.0f,
			0.0f));
}
