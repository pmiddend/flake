#include "float_handling.cl"
#include "positions.cl"

bool
is_solid(
	global float const *boundary,
	size_t const index)
{
	return boundary[index] > FLAKE_REAL_LIT(0.2);
}

kernel void
advect(
	global flake_real2 const *input,
	global flake_real2 *output,
	global flake_real const *boundary,
	int buffer_width,
	flake_real const dt,
	flake_real const grid_scale)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	size_t const current_index =
		FLAKE_AT(buffer_width,position);

	if(is_solid(boundary,current_index))
	{
		output[current_index] =
			(float2)(
				FLAKE_REAL_LIT(0.0));
		return;
	}

	// Retrieve the (imagined) particle's current velocity.
	flake_real2 const current_velocity =
		input[current_index];

	// From the current position, assume the particle moves "back
	// in time". Use its current velocity and move it by -dt.
	flake_real2 const advected_vector =
		FLAKE_CONVERT_REAL2(position) -
		dt *
		(FLAKE_REAL_LIT(1.0) / grid_scale) *
		current_velocity;

	// Clamp to the image space
	flake_real2 const clamped_advected_vector =
		(flake_real2)(
			clamp(
				advected_vector,
				(flake_real2)(
					FLAKE_REAL_LIT(0.0)),
				(flake_real2)(
					buffer_width-1)));

	// Take left top and figure out the neighbors of the advected vector.
	int2 const left_top_advected =
		convert_int2(
			clamped_advected_vector);

	flake_real2 const
		left =
			input[current_index],
		right =
			input[FLAKE_RIGHT_OF(buffer_width,left_top_advected)],
		leftbottom =
			input[FLAKE_BOTTOM_OF(buffer_width,left_top_advected)],
		rightbottom =
			input[FLAKE_RIGHT_BOTTOM_OF(buffer_width,left_top_advected)];

	flake_real2
		floors,
		fractions =
			fract(
				clamped_advected_vector,
				&floors);

	// After moving back in time, we do not always end up exactly at a grid
	// cell's center, we might be inbetween grid cells. Luckily, we can
	// interpolate.
	//
	// Also note that this is the place where "diffusion" occurs. At cells with
	// zero velocity, the only thing we do is take the current position as an
	// integer, convert it to real and subtract (0,0) from it. Then we write it back.
	// However, the conversion to real loses information and we end up with a
	// slighly off-grid position. Together with linear interpolation, this leads to
	// diffusion.
	output[current_index] =
		mix(
			mix(
				left,
				right,
				fractions.x),
			mix(
				leftbottom,
				rightbottom,
				fractions.x),
			fractions.y);
}

/*
This function iterates over all the left border pixels of the image and assigns
a fixed force vector to them. This vector points to the right and has a given
magnitude.
*/
kernel void
apply_external_forces(
	/* 0 */global flake_real2 *input,
	/* 1 */flake_real const force_magnitude,
	/* 2 */int const buffer_width)
{
	size_t const i =
		get_global_id(
			0);

	// Left border (fill with force magnitude)
	input[buffer_width * i] =
		(flake_real2)(
			force_magnitude,
			FLAKE_REAL_LIT(0.0));

	// Top border (fill with top+1 border)
	input[i] =
		input[buffer_width + i];

	// Bottom border (fill with bottom-1 border)
	input[(buffer_width-1) * buffer_width + i] =
		input[(buffer_width-2) * buffer_width + i];

	// Right border (fill with right-1 border)
	input[i * buffer_width + (buffer_width-1)] =
		input[i * buffer_width + (buffer_width-2)];
}

kernel void
divergence(
	global flake_real2 const *input,
	global flake_real *output,
	global flake_real const *boundary,
	int const buffer_width,
	flake_real const grid_scale)
{
	int2 const currentpos =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	int const
		current_index =
			FLAKE_AT(buffer_width,currentpos),
		left_index =
			FLAKE_LEFT_OF(buffer_width,currentpos),
		right_index =
			FLAKE_RIGHT_OF(buffer_width,currentpos),
		top_index =
			FLAKE_TOP_OF(buffer_width,currentpos),
		bottom_index =
			FLAKE_BOTTOM_OF(buffer_width,currentpos);

	flake_real2 const
		left =
			(FLAKE_REAL_LIT(1.0) - boundary[left_index]) * input[left_index],
		right =
			(FLAKE_REAL_LIT(1.0) - boundary[right_index]) * input[right_index],
		top =
			(FLAKE_REAL_LIT(1.0) - boundary[top_index]) * input[top_index],
		bottom =
			(FLAKE_REAL_LIT(1.0) - boundary[bottom_index]) * input[bottom_index];

	output[current_index] =
		((right.x - left.x) + (top.y - bottom.y)) / (FLAKE_REAL_LIT(2.0) * grid_scale);
}

// Calculate the gradient of p and calculate
// w - gradient(p)
kernel void
gradient_and_subtract(
	/* 0 */global flake_real const *p,
	/* 1 */flake_real const grid_scale,
	/* 2 */global flake_real2 const *w,
	/* 3 */global flake_real const *boundary,
	/* 4 */global flake_real2 *output,
	/* 5 */int const buffer_width)
{
	int2 const currentpos =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	int const
		current_index =
			FLAKE_AT(buffer_width,currentpos),
		left_index =
			FLAKE_LEFT_OF(buffer_width,currentpos),
		right_index =
			FLAKE_RIGHT_OF(buffer_width,currentpos),
		top_index =
			FLAKE_TOP_OF(buffer_width,currentpos),
		bottom_index =
			FLAKE_BOTTOM_OF(buffer_width,currentpos);

	// Could be replaced by a mix on the output value below (but that would
	// mean a lot of computations for returning 0 in the end)
	if(is_solid(boundary,current_index))
	{
		output[current_index] =
			(flake_real2)(
				FLAKE_REAL_LIT(0.0));
		return;
	}

	flake_real
		center =
			p[current_index],
		left =
			p[left_index],
		right =
			p[right_index],
		top =
			p[top_index],
		bottom =
			p[bottom_index];

	flake_real2 vmask =
		(flake_real2)(
			FLAKE_REAL_LIT(1.0));

	// Use mix here? What about the vmask?
	if(is_solid(boundary,left_index))
	{
		left = center;
		vmask.x = FLAKE_REAL_LIT(0.0);
	}

	if(is_solid(boundary,right_index))
	{
		right = center;
		vmask.x = FLAKE_REAL_LIT(0.0);
	}

	if(is_solid(boundary,top_index))
	{
		top = center;
		vmask.y = FLAKE_REAL_LIT(0.0);
	}

	if(is_solid(boundary,bottom_index))
	{
		bottom = center;
		vmask.y = FLAKE_REAL_LIT(0.0);
	}

	flake_real2 const velocity =
		w[current_index];

	flake_real2 const pressure_gradient =
		(FLAKE_REAL_LIT(0.5) / grid_scale) *
			(flake_real2)(
				right-left,
				top-bottom);

	flake_real2 result =
		vmask * (velocity - pressure_gradient);

	output[current_index] = result;
}

kernel void
laplacian_residual_absolute_value(
	/* 0 */global flake_real const *rhs,
	/* 1 */global flake_real const *boundary,
	/* 2 */global flake_real const *from,
	/* 3 */global flake_real *to,
	/* 4 */flake_real const grid_scale,
	/* 5 */int const buffer_width)
{
	int2 const currentpos =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	int const
		current_index =
			FLAKE_AT(buffer_width,currentpos),
		left_index =
			FLAKE_LEFT_OF(buffer_width,currentpos),
		right_index =
			FLAKE_RIGHT_OF(buffer_width,currentpos),
		top_index =
			FLAKE_TOP_OF(buffer_width,currentpos),
		bottom_index =
			FLAKE_BOTTOM_OF(buffer_width,currentpos);

	flake_real const
		center =
			from[current_index],
		left =
			mix(
				center,
				from[left_index],
				boundary[left_index]),
		right =
			mix(
				center,
				from[right_index],
				boundary[right_index]),
		top =
			mix(
				center,
				from[top_index],
				boundary[top_index]),
		bottom =
			mix(
				center,
				from[bottom_index],
				boundary[bottom_index]);

	flake_real const
		laplace =
			(left + right + top + bottom - FLAKE_REAL_LIT(4.0) * center) / (grid_scale * grid_scale),
		rhs_value =
			rhs[current_index];

	to[current_index] = laplace - rhs_value;
}
