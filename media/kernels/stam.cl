sampler_t const absolute_clamping_nearest =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_NEAREST;

sampler_t const absolute_clamping_linear =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_LINEAR;

__kernel void
null_image(
	__global __write_only image2d_t f)
{
	write_imagef(
		f,
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1)),
		(float4)(
			0.0f,
			0.0f,
			0.0f,
			0.0f));
}

__kernel void
copy_boundary(
	__global __read_only image2d_t boundary,
	__global __write_only image2d_t vector_field)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	// Branching = not good!
	if(read_imagei(boundary,absolute_clamping_nearest,position).x != 0)
		write_imagef(
			vector_field,
			position,
			(float4)(0.0f,0.0f,0.0f,0.0f));
}

__kernel void
advect(
	__global __read_only image2d_t input,
	__global __write_only image2d_t output,
	float const dt,
	float const grid_size)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

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
		(1.0f / grid_size) *
		current_velocity;

	// After moving back in time, we do not always end up exactly at a grid
	// cell's center, we might be inbetween grid cells. Luckily, we can
	// interpolate (notice the linear interpolation here)!
	float4 const interpolated_vector =
		read_imagef(
			input,
			absolute_clamping_linear,
			advected_vector);

	write_imagef(
		output,
		position,
		interpolated_vector);
}

/*
	This function iterates over all the left border pixels of the image and
	assigns a fixed force vector to them. This vector points to the right and has
  a given magnitude.
 */
__kernel void
apply_external_forces(
	__global __write_only image2d_t output,
	float const force_magnitude,
	unsigned start,
	unsigned end,
	float const grid_size)
{
	size_t const y =
		start + 
		get_global_id(
			0);

	write_imagef(
		output,
		(int2)(0,y),
		(float4)(
			force_magnitude,
			0.0f
			/*grid_size * (y - middle)*/,
			0.0f,
			0.0f));
	/*
	float const middle = 
		(start + end) / 2.0f;

	write_imagef(
		output,
		(int2)(0,y),
		(float4)(
			force_magnitude,
			grid_size * (y - middle),
			0.0f,
			0.0f));
	*/
}

__kernel void
divergence(
	__global __read_only image2d_t input,
	__global __write_only image2d_t output,
	float const grid_size)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	float4 const
		l =
			read_imagef(
				input,
				absolute_clamping_nearest,
				position - (int2)(1,0)),
		r =
			read_imagef(
				input,
				absolute_clamping_nearest,
				position + (int2)(1,0)),
		t =
			read_imagef(
				input,
				absolute_clamping_nearest,
				position + (int2)(0,1)),
		b =
			read_imagef(
				input,
				absolute_clamping_nearest,
				position - (int2)(0,1));

	write_imagef(
		output,
		position,
		(float4)(
			((r.x - l.x) + (t.y - b.y)) / (2.0f * grid_size),
			0.0f,
			0.0f,
			0.0f));
}

// Solves Ax=b for x, where A is the Laplace Operator.
// Explained (in German):
// http://mo.mathematik.uni-stuttgart.de/kurse/kurs5/seite36.html
// Convergence: Extremely slow
__kernel void
jacobi(
	__global __read_only image2d_t b,
	__global __read_only image2d_t x,
	__global __write_only image2d_t output,
	float const alpha,
	float const beta)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	// NOTE: Is float4 the fastest here? Should we extract the .x component first?
	float4 const
		left =
			read_imagef(
				x,
				absolute_clamping_nearest,
				position - (int2)(1,0)),
		right =
			read_imagef(
				x,
				absolute_clamping_nearest,
				position + (int2)(1,0)),
		top =
			read_imagef(
				x,
				absolute_clamping_nearest,
				position + (int2)(0,1)),
		bottom =
			read_imagef(
				x,
				absolute_clamping_nearest,
				position - (int2)(0,1));

	float4 const b_value =
			read_imagef(
				b,
				absolute_clamping_nearest,
				position);

	write_imagef(
		output,
		position,
		(left + right + top + bottom + alpha * b_value) * beta);
}

// Calculate the gradient of p and calculate
// w - gradient(p)
__kernel void
gradient_and_subtract(
	__global __read_only image2d_t p,
	float const grid_size,
	__global __read_only image2d_t w,
	__global __write_only image2d_t output)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	// NOTE: Is float4 the fastest here? Should we extract the .x component first?
	float const
		l =
			read_imagef(
				p,
				absolute_clamping_nearest,
				position - (int2)(1,0)).x,
		r =
			read_imagef(
				p,
				absolute_clamping_nearest,
				position + (int2)(1,0)).x,
		t =
			read_imagef(
				p,
				absolute_clamping_nearest,
				position + (int2)(0,1)).x,
		b =
			read_imagef(
				p,
				absolute_clamping_nearest,
				position - (int2)(0,1)).x;

	write_imagef(
		output,
		position,
		read_imagef(
				w,
				absolute_clamping_nearest,
				position) -
		(0.5f / grid_size) *
		(float4)(
			r-l,
			t-b,
			0.0f,
			0.0f));
}

__kernel void
copy_image(
	__global __read_only image2d_t from,
	__global __write_only image2d_t to)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	write_imagef(
		to,
		position,
		read_imagef(
			from,
			absolute_clamping_nearest,
			position));
}
