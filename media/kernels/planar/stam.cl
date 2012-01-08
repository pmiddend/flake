#include "float_handling.cl"
#include "positions.cl"

bool
is_solid(
	global flake_real const *boundary,
	size_t const index)
{
	return boundary[index] > FLAKE_REAL_LIT(0.5);
}

kernel void
advect(
	global flake_real2 const *input,
	global flake_real2 *output,
	global flake_real const *boundary,
	int const buffer_width,
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
		FLAKE_PLANAR_AT(buffer_width,position);

	if(is_solid(boundary,current_index))
	{
		output[current_index] =
			(flake_real2)(
				FLAKE_REAL_LIT(0.0));
		return;
	}

	flake_real2 const current_velocity =
		input[current_index];

	flake_real2 const advected_vector =
		FLAKE_CONVERT_REAL2(position) -
		dt *
		(FLAKE_REAL_LIT(1.0) / grid_scale) *
		current_velocity;

	int2 advected_lefttop =
		(int2)(
			(int)floor(advected_vector.x),
			(int)floor(advected_vector.y));

	int2 clamped_advected_vector =
		clamp(
			advected_lefttop,
			(int2)(0,0),
			(int2)(buffer_width-1,buffer_width-1));

	flake_real2
		floors,
		fractions =
			fract(
				advected_vector,
				&floors);


	size_t const index_lefttop =
		FLAKE_PLANAR_AT(buffer_width,clamped_advected_vector);

	flake_real2 const
		left =
			input[index_lefttop],
		right =
			input[FLAKE_PLANAR_RIGHT_OF(buffer_width,clamped_advected_vector)],
		leftbottom =
			input[FLAKE_PLANAR_BOTTOM_OF(buffer_width,clamped_advected_vector)],
		rightbottom =
			input[FLAKE_PLANAR_RIGHT_BOTTOM_OF(buffer_width,clamped_advected_vector)];

	output[current_index] =
		/*
		(1.0f - fractions.x) * (1.0f - fractions.y) * left +
		fractions.x * (1.0f - fractions.y) * right +
		(1.0f - fractions.x) * fractions.y * leftbottom +
		fractions.x * fractions.y * rightbottom;
		*/
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
			FLAKE_PLANAR_AT(buffer_width,currentpos),
		left_index =
			FLAKE_PLANAR_LEFT_OF(buffer_width,currentpos),
		right_index =
			FLAKE_PLANAR_RIGHT_OF(buffer_width,currentpos),
		top_index =
			FLAKE_PLANAR_TOP_OF(buffer_width,currentpos),
		bottom_index =
			FLAKE_PLANAR_BOTTOM_OF(buffer_width,currentpos);

	flake_real2
		left =
//			(FLAKE_REAL_LIT(1.0) - boundary[left_index]) * input[left_index],
			input[left_index],
		right =
//			(FLAKE_REAL_LIT(1.0) - boundary[right_index]) * input[right_index],
			input[right_index],
		top =
//			(FLAKE_REAL_LIT(1.0) - boundary[top_index]) * input[top_index],
			input[top_index],
		bottom =
//			(FLAKE_REAL_LIT(1.0) - boundary[bottom_index]) * input[bottom_index];
			input[bottom_index];

	if(is_solid(boundary,left_index))
		left = (float2)(0.0f);
	if(is_solid(boundary,right_index))
		right = (float2)(0.0f);
	if(is_solid(boundary,top_index))
		top = (float2)(0.0f);
	if(is_solid(boundary,bottom_index))
		bottom = (float2)(0.0f);

	output[current_index] =
//		((right.x - left.x) + (top.y - bottom.y)) / (FLAKE_REAL_LIT(2.0) * grid_scale);
		((right.x - left.x) + (bottom.y - top.y)) / (FLAKE_REAL_LIT(2.0) * grid_scale);
}

// Calculate the gradient of p and calculate
// w - gradient(p)
kernel void
gradient(
	/* 0 */global flake_real const *p,
	/* 1 */global flake_real const *boundary,
	/* 2 */global flake_real2 *output,
	/* 3 */int const buffer_width,
	/* 4 */flake_real const grid_scale)
{
	int2 const currentpos =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	int const
		current_index =
			FLAKE_PLANAR_AT(buffer_width,currentpos),
		left_index =
			FLAKE_PLANAR_LEFT_OF(buffer_width,currentpos),
		right_index =
			FLAKE_PLANAR_RIGHT_OF(buffer_width,currentpos),
		top_index =
			FLAKE_PLANAR_TOP_OF(buffer_width,currentpos),
		bottom_index =
			FLAKE_PLANAR_BOTTOM_OF(buffer_width,currentpos);

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

	// Use mix here? What about the vmask?
	if(is_solid(boundary,left_index))
		left = center;

	if(is_solid(boundary,right_index))
		right = center;

	if(is_solid(boundary,top_index))
		top = center;

	if(is_solid(boundary,bottom_index))
		bottom = center;

	flake_real2 const pressure_gradient =
		10.0f * (FLAKE_REAL_LIT(0.5) / grid_scale) *
			(flake_real2)(
				right-left,
				bottom-top);

	output[current_index] = pressure_gradient;
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
			FLAKE_PLANAR_AT(buffer_width,currentpos),
		left_index =
			FLAKE_PLANAR_LEFT_OF(buffer_width,currentpos),
		right_index =
			FLAKE_PLANAR_RIGHT_OF(buffer_width,currentpos),
		top_index =
			FLAKE_PLANAR_TOP_OF(buffer_width,currentpos),
		bottom_index =
			FLAKE_PLANAR_BOTTOM_OF(buffer_width,currentpos);

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
				bottom-top);

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
			FLAKE_PLANAR_AT(buffer_width,currentpos),
		left_index =
			FLAKE_PLANAR_LEFT_OF(buffer_width,currentpos),
		right_index =
			FLAKE_PLANAR_RIGHT_OF(buffer_width,currentpos),
		top_index =
			FLAKE_PLANAR_TOP_OF(buffer_width,currentpos),
		bottom_index =
			FLAKE_PLANAR_BOTTOM_OF(buffer_width,currentpos);

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
