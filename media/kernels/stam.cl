#include "float_handling.cl"
#include "positions.cl"

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

bool
is_solid(
	global read_only image2d_t boundary,
	int2 const position)
{
	return
		read_imagei(
			boundary,
			absolute_clamping_nearest,
			position).x != 0;
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
		FLAKE_WRITE_IMAGE_FUNCTION(
			output,
			position,
			(flake_real4)(FLAKE_REAL_LIT(0.0)));
		return;
	}

	// Retrieve the (imagined) particle's current velocity. We can
	// get this using a nearest filter, since we're at an exact
	// pixel location (given by the kernel's global id)
	flake_real2 const current_velocity =
		FLAKE_READ_IMAGE_FUNCTION(
			input,
			absolute_clamping_nearest,
			position).xy;

	// From the current position, assume the particle moves "back
	// in time". Use its current velocity and move it by -dt.
	flake_real2 const advected_vector =
		FLAKE_CONVERT_REAL2(position) -
		FLAKE_FROM_FLOAT(dt) *
		(FLAKE_REAL_LIT(1.0) / FLAKE_FROM_FLOAT(grid_scale)) *
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
	// integer, convert it to real and subtract (0,0) from it. Then we write it back.
	// However, the conversion to real loses information and we end up with a
	// slighly off-grid position. Together with linear interpolation, this leads to
	// diffusion.
	//
	// Also note that using CLAMP_TO_EDGE leads to border vectors on the left
	// that point to the right (zero y coordinate) to not be changed.
	flake_real4 const interpolated_vector =
		FLAKE_READ_IMAGE_FUNCTION(
			input,
			absolute_clamping_linear,
			// Linear interpolation uses (u,v) - (0.5,0.5) as "reference", so
			// compensate for that using an addition here.
			advected_vector + (flake_real2)(FLAKE_REAL_LIT(0.5)));

	FLAKE_WRITE_IMAGE_FUNCTION(
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
	FLAKE_WRITE_IMAGE_FUNCTION(
		output,
		(int2)(0,i),
		FLAKE_READ_IMAGE_FUNCTION(
			input,
			absolute_clamping_nearest,
			(int2)(1,i)));
		*/
	FLAKE_WRITE_IMAGE_FUNCTION(
		output,
		(int2)(0,i),
		(flake_real4)(
			FLAKE_FROM_FLOAT(force_magnitude),
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0)));

	FLAKE_WRITE_IMAGE_FUNCTION(
		output,
		(int2)(i,0),
		FLAKE_READ_IMAGE_FUNCTION(
			input,
			absolute_clamping_nearest,
			(int2)(i,1)));

	FLAKE_WRITE_IMAGE_FUNCTION(
		output,
		(int2)(edge_size-1,i),
		FLAKE_READ_IMAGE_FUNCTION(
			input,
			absolute_clamping_nearest,
			(int2)(edge_size-2,i)));

	FLAKE_WRITE_IMAGE_FUNCTION(
		output,
		(int2)(i,edge_size-1),
		FLAKE_READ_IMAGE_FUNCTION(
			input,
			absolute_clamping_nearest,
			(int2)(i,edge_size-2)));

	/*
	if(i >= start_y && i < start_y + 2*fan_width)
	{
		FLAKE_WRITE_IMAGE_FUNCTION(
			output,
			(int2)(0,i),
			(flake_real4)(
				fast_normalize(
					(flake_real2)(
						force_magnitude,
						i - middle)),
				FLAKE_REAL_LIT(0.0),
				FLAKE_REAL_LIT(0.0)));
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

	flake_real2
		left =
			FLAKE_READ_IMAGE_FUNCTION(
				input,
				absolute_clamping_nearest,
				position + pos_left).xy,
		right =
			FLAKE_READ_IMAGE_FUNCTION(
				input,
				absolute_clamping_nearest,
				position + pos_right).xy,
		top =
			FLAKE_READ_IMAGE_FUNCTION(
				input,
				absolute_clamping_nearest,
				position + pos_top).xy,
		bottom =
			FLAKE_READ_IMAGE_FUNCTION(
				input,
				absolute_clamping_nearest,
				position + pos_bottom).xy;

	if(is_solid(boundary,position + pos_left))
		left = (flake_real2)(FLAKE_REAL_LIT(0.0));
	if(is_solid(boundary,position + pos_right))
		right = (flake_real2)(FLAKE_REAL_LIT(0.0));
	if(is_solid(boundary,position + pos_top))
		top = (flake_real2)(FLAKE_REAL_LIT(0.0));
	if(is_solid(boundary,position + pos_bottom))
		bottom = (flake_real2)(FLAKE_REAL_LIT(0.0));

	FLAKE_WRITE_IMAGE_FUNCTION(
		output,
		position,
		(flake_real4)(
			((right.x - left.x) + (top.y - bottom.y)) / (FLAKE_REAL_LIT(2.0) * FLAKE_FROM_FLOAT(grid_scale)),
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0)));
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

	flake_real const
		left_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_left).x,
		right_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_right).x,
		top_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_bottom).x;

	flake_real
		center =
			FLAKE_READ_IMAGE_FUNCTION(
				x,
				absolute_clamping_nearest,
				position).x,
		left =
			left_boundary *
			center +
			(FLAKE_REAL_LIT(1.0) - left_boundary) *
			FLAKE_READ_IMAGE_FUNCTION(
				x,
				absolute_clamping_nearest,
				position + pos_left).x,
		right =
			right_boundary *
			center +
			(FLAKE_REAL_LIT(1.0) - right_boundary) *
			FLAKE_READ_IMAGE_FUNCTION(
				x,
				absolute_clamping_nearest,
				position + pos_right).x,
		top =
			top_boundary *
			center +
			(FLAKE_REAL_LIT(1.0) - top_boundary) *
			FLAKE_READ_IMAGE_FUNCTION(
				x,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom =
			bottom_boundary *
			center +
			(FLAKE_REAL_LIT(1.0) - bottom_boundary) *
			FLAKE_READ_IMAGE_FUNCTION(
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

	flake_real const b_value =
		FLAKE_READ_IMAGE_FUNCTION(
			b,
			absolute_clamping_nearest,
			position).x;

	FLAKE_WRITE_IMAGE_FUNCTION(
		output,
		position,
		(flake_real4)(
			(left + right + top + bottom + FLAKE_FROM_FLOAT(alpha) * b_value) * FLAKE_FROM_FLOAT(beta),
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0)));
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
		FLAKE_WRITE_IMAGE_FUNCTION(
			output,
			position,
			(flake_real4)(
				FLAKE_REAL_LIT(0.0)));
		return;
	}

	flake_real
		center =
			FLAKE_READ_IMAGE_FUNCTION(
				p,
				absolute_clamping_nearest,
				position).x,
		left =
			FLAKE_READ_IMAGE_FUNCTION(
				p,
				absolute_clamping_nearest,
				position + pos_left).x,
		right =
			FLAKE_READ_IMAGE_FUNCTION(
				p,
				absolute_clamping_nearest,
				position + pos_right).x,
		top =
			FLAKE_READ_IMAGE_FUNCTION(
				p,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom =
			FLAKE_READ_IMAGE_FUNCTION(
				p,
				absolute_clamping_nearest,
				position + pos_bottom).x;

	flake_real2 vmask =
		(flake_real2)(FLAKE_REAL_LIT(1.0));

	if(is_solid(boundary,position + pos_left))
	{
		left = center;
		vmask.x = FLAKE_REAL_LIT(0.0);
	}

	if(is_solid(boundary,position + pos_right))
	{
		right = center;
		vmask.x = FLAKE_REAL_LIT(0.0);
	}

	if(is_solid(boundary,position + pos_top))
	{
		top = center;
		vmask.y = FLAKE_REAL_LIT(0.0);
	}

	if(is_solid(boundary,position + pos_bottom))
	{
		bottom = center;
		vmask.y = FLAKE_REAL_LIT(0.0);
	}

	flake_real2 const velocity =
		FLAKE_READ_IMAGE_FUNCTION(
			w,
			absolute_clamping_nearest,
			position).xy;

	flake_real2 const pressure_gradient =
		(FLAKE_REAL_LIT(0.5) / FLAKE_FROM_FLOAT(grid_scale)) *
			(flake_real2)(
				right-left,
				top-bottom);

	flake_real2 result =
		velocity - pressure_gradient;

	result = vmask * result;

	FLAKE_WRITE_IMAGE_FUNCTION(
		output,
		position,
		(flake_real4)(result.xy,FLAKE_REAL_LIT(0.0),FLAKE_REAL_LIT(0.0)));
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

	flake_real const
		left_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_left).x,
		right_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_right).x,
		top_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_bottom).x;

	flake_real
		center =
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position).x,
		left =
			left_boundary *
			center +
			(FLAKE_REAL_LIT(1.0) - left_boundary) *
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position + pos_left).x,
		right =
			right_boundary *
			center +
			(FLAKE_REAL_LIT(1.0) - right_boundary) *
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position + pos_right).x,
		top =
			top_boundary *
			center +
			(FLAKE_REAL_LIT(1.0) - top_boundary) *
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom =
			bottom_boundary *
			center +
			(FLAKE_REAL_LIT(1.0) - bottom_boundary) *
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position + pos_bottom).x;

	flake_real const
		laplace =
			(left + right + top + bottom - FLAKE_REAL_LIT(4.0) * center) / (grid_scale * grid_scale),
		rhs_value =
			FLAKE_READ_IMAGE_FUNCTION(
				rhs,
				absolute_clamping_nearest,
				position).x;

	FLAKE_WRITE_IMAGE_FUNCTION(
		to,
		position,
		(flake_real4)(
			laplace - rhs_value,
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0)));
}
