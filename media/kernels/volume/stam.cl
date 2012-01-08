#include "volume/positions.cl"

bool
is_solid(
	global float const *boundary,
	size_t const index)
{
	return boundary[index] > 0.5f;
}

kernel void
advect(
	global float4 const *input,
	global float4 *output,
	global float const *boundary,
	int const buffer_width,
	float const dt,
	float const grid_scale)
{
	int3 const position =
		(int3)(
			get_global_id(0),
			get_global_id(1),
			get_global_id(2));

	size_t const current_index =
		FLAKE_VOLUME_AT(buffer_width,position);

	if(is_solid(boundary,current_index))
	{
		output[current_index] =
			(float4)(
				0.0f);
		return;
	}

	float4 const current_velocity =
		input[current_index];

	float4 const advected_vector =
		(float4)(position.x,position.y,position.z,0.0f) -
		dt *
		(1.0f / grid_scale) *
		current_velocity;

	int3 advected_lefttopback =
		(int3)(
			(int)floor(advected_vector.x),
			(int)floor(advected_vector.y),
			(int)floor(advected_vector.z));

	int3
		clamped_advected_back_vector =
			clamp(
				advected_lefttopback,
				(int3)(0,0,0),
				(int3)(buffer_width-1,buffer_width-1,buffer_width-1)),
		clamped_advected_front_vector =
			(int3)(
				clamped_advected_back_vector.x,
				clamped_advected_back_vector.y,
				min(
					buffer_width-1,
					clamped_advected_back_vector.z+1));

	float4
		floors,
		fractions =
			fract(
				advected_vector,
				&floors);

	float4 const
		lefttopback =
			input[FLAKE_VOLUME_AT(buffer_width,clamped_advected_back_vector)],
		righttopback =
			input[FLAKE_VOLUME_RIGHT_OF(buffer_width,clamped_advected_back_vector)],
		leftbottomback =
			input[FLAKE_VOLUME_BOTTOM_OF(buffer_width,clamped_advected_back_vector)],
		rightbottomback =
			input[FLAKE_VOLUME_RIGHT_BOTTOM_OF(buffer_width,clamped_advected_back_vector)],
		lefttopfront =
			input[FLAKE_VOLUME_AT(buffer_width,clamped_advected_front_vector)],
		righttopfront =
			input[FLAKE_VOLUME_RIGHT_OF(buffer_width,clamped_advected_front_vector)],
		leftbottomfront =
			input[FLAKE_VOLUME_BOTTOM_OF(buffer_width,clamped_advected_front_vector)],
		rightbottomfront =
			input[FLAKE_VOLUME_RIGHT_BOTTOM_OF(buffer_width,clamped_advected_front_vector)];

	output[current_index] =
		mix(
			mix(
				mix(
					lefttopback,
					righttopback,
					fractions.x),
				mix(
					leftbottomback,
					rightbottomback,
					fractions.x),
				fractions.y),
			mix(
				mix(
					lefttopfront,
					righttopfront,
					fractions.x),
				mix(
					leftbottomfront,
					rightbottomfront,
					fractions.x),
				fractions.y),
			fractions.z);
}

/*
This function iterates over all the left border pixels of the image and assigns
a fixed force vector to them. This vector points to the right and has a given
magnitude.
*/
kernel void
apply_external_forces(
	/* 0 */global float4 *input,
	/* 1 */float const force_magnitude,
	/* 2 */int const w)
{
	int2 const pos =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	int3 volposfrom,volposto;

	volposfrom =
		(int3)(
			0,
			pos.x,
			pos.y);

	// Left border (fill with force magnitude)
	input[FLAKE_VOLUME_AT(w,volposfrom)] =
		(float4)(
			force_magnitude,
			0.0f,
			0.0f,
			0.0f);

	// Top border (fill with top+1 border)
	volposfrom =
		(int3)(
			pos.x,
			1,
			pos.y);

	volposto =
		(int3)(
			pos.x,
			0,
			pos.y);

	input[FLAKE_VOLUME_AT(w,volposto)] =
		input[FLAKE_VOLUME_AT(w,volposfrom)];

	// Bottom border (fill with bottom-1 border)
	volposfrom =
		(int3)(
			pos.x,
			w-2,
			pos.y);

	volposto =
		(int3)(
			pos.x,
			w-1,
			pos.y);

	input[FLAKE_VOLUME_AT(w,volposto)] =
		input[FLAKE_VOLUME_AT(w,volposfrom)];

	// Right border
	volposfrom =
		(int3)(
			w-2,
			pos.x,
			pos.y);

	volposto =
		(int3)(
			w-1,
			pos.x,
			pos.y);

	input[FLAKE_VOLUME_AT(w,volposto)] =
		input[FLAKE_VOLUME_AT(w,volposfrom)];

	// Front border
	volposfrom =
		(int3)(
			pos.x,
			pos.y,
			w-2);

	volposto =
		(int3)(
			pos.x,
			pos.y,
			w-1);

	input[FLAKE_VOLUME_AT(w,volposto)] =
		input[FLAKE_VOLUME_AT(w,volposfrom)];

	// Back border
	volposfrom =
		(int3)(
			pos.x,
			pos.y,
			1);

	volposto =
		(int3)(
			pos.x,
			pos.y,
			0);

	input[FLAKE_VOLUME_AT(w,volposto)] =
		input[FLAKE_VOLUME_AT(w,volposfrom)];
}

kernel void
divergence(
	global float4 const *input,
	global float *output,
	global float const *boundary,
	int const buffer_width,
	float const grid_scale)
{
	int3 const currentpos =
		(int3)(
			get_global_id(
				0),
			get_global_id(
				1),
			get_global_id(
				2));

	int const
		current_index =
			FLAKE_VOLUME_AT(buffer_width,currentpos),
		left_index =
			FLAKE_VOLUME_LEFT_OF(buffer_width,currentpos),
		right_index =
			FLAKE_VOLUME_RIGHT_OF(buffer_width,currentpos),
		top_index =
			FLAKE_VOLUME_TOP_OF(buffer_width,currentpos),
		bottom_index =
			FLAKE_VOLUME_BOTTOM_OF(buffer_width,currentpos),
		front_index =
			FLAKE_VOLUME_FRONT_OF(buffer_width,currentpos),
		back_index =
			FLAKE_VOLUME_BACK_OF(buffer_width,currentpos);

	float4
		left =
			(1.0f - boundary[left_index]) * input[left_index],
		right =
			(1.0f - boundary[right_index]) * input[right_index],
		top =
			(1.0f - boundary[top_index]) * input[top_index],
		bottom =
			(1.0f - boundary[bottom_index]) * input[bottom_index],
		back =
			(1.0f - boundary[back_index]) * input[back_index],
		front =
			(1.0f - boundary[front_index]) * input[front_index];

	output[current_index] =
		// FIXME back.z - front.z might be an error, try if everything else fails
		((right.x - left.x) + (bottom.y - top.y) + (back.z - front.z)) / (2.0f * grid_scale);
}

// Calculate the gradient of p and calculate
// w - gradient(p)
kernel void
gradient_and_subtract(
	/* 0 */global float const *p,
	/* 1 */float const grid_scale,
	/* 2 */global float4 const *w,
	/* 3 */global float const *boundary,
	/* 4 */global float4 *output,
	/* 5 */int const buffer_width)
{
	int3 const currentpos =
		(int3)(
			get_global_id(
				0),
			get_global_id(
				1),
			get_global_id(
				2));

	int const
		current_index =
			FLAKE_VOLUME_AT(buffer_width,currentpos),
		left_index =
			FLAKE_VOLUME_LEFT_OF(buffer_width,currentpos),
		right_index =
			FLAKE_VOLUME_RIGHT_OF(buffer_width,currentpos),
		top_index =
			FLAKE_VOLUME_TOP_OF(buffer_width,currentpos),
		bottom_index =
			FLAKE_VOLUME_BOTTOM_OF(buffer_width,currentpos),
		back_index =
			FLAKE_VOLUME_BACK_OF(buffer_width,currentpos),
		front_index =
			FLAKE_VOLUME_FRONT_OF(buffer_width,currentpos);

	// Could be replaced by a mix on the output value below (but that would
	// mean a lot of computations for returning 0 in the end)
	if(is_solid(boundary,current_index))
	{
		output[current_index] =
			(float4)(
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
			p[bottom_index],
		back =
			p[back_index],
		front =
			p[front_index];

	float4 vmask =
		(float4)(
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

	if(is_solid(boundary,back_index))
	{
		back = center;
		vmask.y = 0.0f;
	}

	if(is_solid(boundary,front_index))
	{
		front = center;
		vmask.y = 0.0f;
	}

	float4 const velocity =
		w[current_index];

	float4 const pressure_gradient =
		(0.5f / grid_scale) *
			(float4)(
				right-left,
				bottom-top,
				back-front,
				0.0f);

	float4 result =
		vmask * (velocity - pressure_gradient);

	output[current_index] = result;
}
