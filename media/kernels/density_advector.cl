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
advect(
	global read_only image2d_t input,
	global write_only image2d_t output,
	global read_only image2d_t velocity,
	float const dt,
	float const grid_scale)
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
			velocity,
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
	float4 const interpolated_density =
		read_imagef(
			input,
			absolute_clamping_linear,
			// Linear interpolation uses (u,v) - (0.5,0.5) as "reference", so
			// compensate for that using an addition here.
			advected_vector + (float2)(0.5f,0.5f));

	write_imagef(
		output,
		position,
		interpolated_density);
}

kernel void
apply_sources(
	global read_only image2d_t sources,
	global write_only image2d_t density)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	if(
		read_imagef(
			sources,
			absolute_clamping_nearest,
			position).x > 0.5f)
		write_imagef(
			density,
			position,
			(float4)(
				1.0f));
}
