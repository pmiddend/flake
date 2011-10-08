float2 const
bilinear_interpolation(
	uint2 const field_size,
	global float2 const *field,
	float2 position)
{
	position = 
		clamp(
			position,
			(float2)(0.0f),
			convert_float2(
				field_size - (float2)(1.0f,1.0f)));

	uint2 const rounded_down = 
		convert_uint2(
			position);

	float2 const
		fractional = 
			position - floor(position),
		lefttop = 
			field[field_size.x * rounded_down.y + rounded_down.x],
		righttop = 
			field[field_size.x * rounded_down.y + rounded_down.x + 1],
		leftbottom = 
			field[field_size.x * (rounded_down.y + 1) + rounded_down.x],
		rightbottom = 
			field[field_size.x * (rounded_down.y + 1) + rounded_down.x + 1],
		top_interpolant =
			fractional.x * lefttop + (1.0f - fractional.x) * righttop,
		bottom_interpolant = 
			fractional.x * leftbottom + (1.0f - fractional.x) * rightbottom;

	return (1.0f - fractional.y) * top_interpolant + fractional.y * bottom_interpolant;
}

bool
is_solid(
	global uchar const *boundary,
	size_t index)
{
	return 
		boundary[index] != 0;
}

kernel void
null_image(
	global float *buffer)
{
	buffer[get_global_id(0)] = 0.0f;
}

kernel void
advect(
	uint2 const field_size,
	global float2 const *input,
	global float2 *output,
	global uchar const *boundary,
	float const dt,
	float const grid_scale)
{
	uint2 const position =
		(uint2)(
			get_global_id(
				0),
			get_global_id(
				1));

	size_t const index = 
		position.y * field_size.x + position.x;

	if(is_solid(boundary,index))
	{
		output[index] = (float2)(0.0f);
		return;
	}

	// Retrieve the (imagined) particle's current velocity. We can
	// get this using a nearest filter, since we're at an exact
	// pixel location (given by the kernel's global id)
	float2 const current_velocity =
		input[index];

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
	output[index] = 
		bilinear_interpolation(
			field_size,
			input,
			advected_vector);
}

/*
	This function iterates over all the left border pixels of the image and
	assigns a fixed force vector to them. This vector points to the right and has
  a given magnitude.
 */
kernel void
apply_external_forces(
	uint2 const field_size,
	global float2 *output,
	float const force_magnitude,
	unsigned start,
	unsigned end,
	float const grid_scale)
{
	size_t const y =
		start + 
		get_global_id(
			0);

	float const middle = 
		(start + end) / 2.0f;

	output[field_size.x * y] = 
		(float2)(
			force_magnitude,
			grid_scale * (y - middle));
}

kernel void
divergence(
	uint2 const field_size,
	global float2 const *input,
	global float *output,
	global uchar const *boundary,
	float const grid_scale)
{
	uint2 const position =
		(uint2)(
			get_global_id(
				0),
			get_global_id(
				1));

	size_t const base_index =
		field_size.x * position.y + position.x;

	size_t const 
		left_index = 
			base_index - (position.x == 0 ? 0 : 1),
		right_index = 
			base_index + (position.x == (field_size.x - 1) ? 0 : 1),
		top_index = 
			base_index - (position.y == 0 ? 0 : 1),
		bottom_index = 
			base_index - (position.y == (field_size.y - 1) ? 0 : 1); 

	float2 const 
		left = 
			is_solid(boundary,left_index) ? input[left_index] : (float2)(0.0f,0.0f),
		right = 
			is_solid(boundary,right_index) ? input[right_index] : (float2)(0.0f,0.0f),
		top = 
			is_solid(boundary,top_index) ? input[top_index] : (float2)(0.0f,0.0f),
		bottom = 
			is_solid(boundary,bottom_index) ? input[bottom_index] : (float2)(0.0f,0.0f);

	output[base_index] = 
		((right.x - left.x) + (top.y - bottom.y)) / (2.0f * grid_scale);
}

// Solves Ax=b for x, where A is the Laplace Operator.
// Explained (in German):
// http://mo.mathematik.uni-stuttgart.de/kurse/kurs5/seite36.html
// Convergence: Extremely slow
kernel void
jacobi(
	/* 0 */uint2 const field_size,
	/* 1 */global float const *b,
	/* 2 */global float const *x,
	/* 3 */global uchar const *boundary,
	/* 4 */global float *output,
	/* 5 */float const alpha,
	/* 6 */float const beta)
{
	uint2 const position =
		(uint2)(
			get_global_id(
				0),
			get_global_id(
				1));

	size_t const base_index =
		field_size.x * position.y + position.x;

	size_t const 
		left_index = 
			base_index - (position.x == 0 ? 0 : 1),
		right_index = 
			base_index + (position.x == (field_size.x - 1) ? 0 : 1),
		top_index = 
			base_index - (position.y == 0 ? 0 : 1),
		bottom_index = 
			base_index - (position.y == (field_size.y - 1) ? 0 : 1); 

	float const 
		center = 
			x[base_index],
		left = 
			is_solid(boundary,left_index) ? x[left_index] : center,
		right = 
			is_solid(boundary,right_index) ? x[right_index] : center,
		top = 
			is_solid(boundary,top_index) ? x[top_index] : center,
		bottom = 
			is_solid(boundary,bottom_index) ? x[bottom_index] : center;

	float const b_value =
		b[base_index];

	output[base_index] = 
		(left + right + top + bottom + alpha * b_value) * beta;
}

// Calculate the gradient of p and calculate
// w - gradient(p)
kernel void
gradient_and_subtract(
	/* 0 */uint2 const field_size,
	/* 1 */global float const *p,
	/* 2 */float const grid_scale,
	/* 3 */global float2 const *w,
	/* 4 */global uchar const *boundary,
	/* 5 */global float2 *output)
{
	uint2 const position =
		(uint2)(
			get_global_id(
				0),
			get_global_id(
				1));

	size_t const base_index =
		field_size.x * position.y + position.x;

	if(is_solid(boundary,base_index))
	{
		output[base_index] = (float2)(0.0f);
		return;
	}

	size_t const 
		left_index = 
			base_index - (position.x == 0 ? 0 : 1),
		right_index = 
			base_index + (position.x == (field_size.x - 1) ? 0 : 1),
		top_index = 
			base_index - (position.y == 0 ? 0 : 1),
		bottom_index = 
			base_index - (position.y == (field_size.y - 1) ? 0 : 1); 

	float
		center = 
			p[base_index],
		left = 
			p[left_index],
		right = 
			p[right_index],
		top = 
			p[top_index],
		bottom = 
			p[bottom_index];

	float2 vmask = 
		(float2)(1.0f,1.0f);

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
		w[base_index];

	float2 const pressure_gradient = 
		(0.5f / grid_scale) *
			(float2)(
				right-left,
				top-bottom);

	float2 result = 
		velocity - pressure_gradient;

	result = vmask * result;

	output[base_index] = 
		result.xy;
}
