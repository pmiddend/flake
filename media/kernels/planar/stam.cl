#include "planar/positions.cl"

bool
is_solid(
	global float const *boundary,
	size_t const index)
{
	return boundary[index] > 0.5f;
}

kernel void
advect(
	global float2 const *input,
	global float2 *output,
	global float const *boundary,
	int const buffer_width,
	float const dt,
	float const grid_scale)
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
			(float2)(
				0.0f);
		return;
	}

	float2 const current_velocity =
		input[current_index];

	float2 const advected_vector =
		convert_float2(position) -
		dt *
		(1.0f / grid_scale) *
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

	float2
		floors,
		fractions =
			fract(
				advected_vector,
				&floors);


	size_t const index_lefttop =
		FLAKE_PLANAR_AT(buffer_width,clamped_advected_vector);

	float2 const
		left =
			input[index_lefttop],
		right =
			input[FLAKE_PLANAR_RIGHT_OF(buffer_width,clamped_advected_vector)],
		leftbottom =
			input[FLAKE_PLANAR_BOTTOM_OF(buffer_width,clamped_advected_vector)],
		rightbottom =
			input[FLAKE_PLANAR_RIGHT_BOTTOM_OF(buffer_width,clamped_advected_vector)];

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

kernel void
maccormack(
	global float2 const *forward,
	global float2 const *backward,
	global float2 const *velocity,
	global float2 *output,
	global float const *boundary,
	float const dt)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	int const buffer_width =
		get_global_size(0);

	size_t const current_index =
		FLAKE_PLANAR_AT(buffer_width,position);

	if(is_solid(boundary,current_index))
	{
		output[current_index] =
			(float2)(
				0.0f);
		return;
	}

	float2 const advected_vector =
		convert_float2(position) -
		dt *
		velocity[current_index];

	int2 advected_lefttop =
		(int2)(
			(int)floor(advected_vector.x),
			(int)floor(advected_vector.y));

	int2 clamped_advected_vector =
		clamp(
			advected_lefttop,
			(int2)(0,0),
			(int2)(buffer_width-1,buffer_width-1));

	size_t const index_lefttop =
		FLAKE_PLANAR_AT(buffer_width,clamped_advected_vector);

	float2 const
		left =
			velocity[index_lefttop],
		right =
			velocity[FLAKE_PLANAR_RIGHT_OF(buffer_width,clamped_advected_vector)],
		leftbottom =
			velocity[FLAKE_PLANAR_BOTTOM_OF(buffer_width,clamped_advected_vector)],
		rightbottom =
			velocity[FLAKE_PLANAR_RIGHT_BOTTOM_OF(buffer_width,clamped_advected_vector)];

	float2 const
		phi_min =
			min(
				left,
				min(
					right,
					min(
						leftbottom,
						rightbottom))),
		phi_max =
			max(
				left,
				max(
					right,
					max(
						leftbottom,
						rightbottom)));

	output[current_index] =
		clamp(
			forward[current_index] +
			0.5f * (velocity[current_index] - backward[current_index]),
			phi_min,
			phi_max);
}

/*
This function iterates over all the left border pixels of the image and assigns
a fixed force vector to them. This vector points to the right and has a given
magnitude.
*/
kernel void
apply_external_forces(
	/* 0 */global float2 *input,
	/* 1 */float const force_magnitude,
	/* 2 */int const buffer_width)
{
	size_t const i =
		get_global_id(
			0);

	// Left border (fill with force magnitude)
	input[buffer_width * i] =
		(float2)(
			force_magnitude,
			0.0f);

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
apply_gravity(
	global float2 *input,
	float const magnitude,
	float const dt)
{
	input[get_global_id(0)] += dt * (float2)(0.0,magnitude);
}

kernel void
divergence(
	global float2 const *input,
	global float *output,
	global float const *boundary,
	int const buffer_width,
	float const grid_scale)
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

	float2
		left =
//			(1.0f - boundary[left_index]) * input[left_index],
			input[left_index],
		right =
//			(1.0f - boundary[right_index]) * input[right_index],
			input[right_index],
		top =
//			(1.0f - boundary[top_index]) * input[top_index],
			input[top_index],
		bottom =
//			(1.0f - boundary[bottom_index]) * input[bottom_index];
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
//		((right.x - left.x) + (top.y - bottom.y)) / (2.0f * grid_scale);
		((right.x - left.x) + (bottom.y - top.y)) / (2.0f * grid_scale);
}

// Calculate the gradient of p and calculate
// w - gradient(p)
kernel void
gradient(
	/* 0 */global float const *p,
	/* 1 */global float const *boundary,
	/* 2 */global float2 *output,
	/* 3 */int const buffer_width,
	/* 4 */float const grid_scale)
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
			(float2)(
				0.0f);
		return;
	}

	float
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

	float2 const pressure_gradient =
		10.0f * (0.5f / grid_scale) *
			(float2)(
				right-left,
				bottom-top);

	output[current_index] = pressure_gradient;
}

// Calculate the gradient of p and calculate
// w - gradient(p)
kernel void
gradient_and_subtract(
	/* 0 */global float const *p,
	/* 1 */float const grid_scale,
	/* 2 */global float2 const *w,
	/* 3 */global float const *boundary,
	/* 4 */global float2 *output,
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
			(float2)(
				0.0f);
		return;
	}

	float
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

	float2 vmask =
		(float2)(
			1.0f);

	// Use mix here? What about the vmask?
	if(is_solid(boundary,left_index))
	{
		left = center;
		vmask.x = 0.0f;
	}

	if(is_solid(boundary,right_index))
	{
		right = center;
		vmask.x = 0.0f;
	}

	if(is_solid(boundary,top_index))
	{
		top = center;
		vmask.y = 0.0f;
	}

	if(is_solid(boundary,bottom_index))
	{
		bottom = center;
		vmask.y = 0.0f;
	}

	float2 const velocity =
		w[current_index];

	float2 const pressure_gradient =
		(0.5f / grid_scale) *
			(float2)(
				right-left,
				bottom-top);

	float2 result =
		vmask * (velocity - pressure_gradient);

	output[current_index] = result;
}

kernel void
laplacian_residual_absolute_value(
	/* 0 */global float const *rhs,
	/* 1 */global float const *boundary,
	/* 2 */global float const *from,
	/* 3 */global float *to,
	/* 4 */float const grid_scale,
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

	float const
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

	float const
		laplace =
			(left + right + top + bottom - 4.0f * center) / (grid_scale * grid_scale),
		rhs_value =
			rhs[current_index];

	to[current_index] = laplace - rhs_value;
}
