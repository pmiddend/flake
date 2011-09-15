sampler_t const absolute_clamping_nearest =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_NEAREST;

__kernel void
fill_vb_with_arrows(
	__global float *vb,
	float const grid_size,
	float const arrow_length,
	__global __read_only image2d_t vector_field)
{
	int2 const this_pos = (int2)(
		get_global_id(0),
		get_global_id(1));

	// Each kernel processes two points. Each point has 6 floats stored.
	// Thus, 2 * 5 floats per kernel.
	size_t const base_index =
		2 * 5 * (this_pos.y * get_image_width(vector_field) + this_pos.x);

	float2 const this_pos_float =
		(float2)(
			(float)this_pos.x,
			(float)this_pos.y);

	float2 const
		start_position =
			grid_size * (this_pos_float + 0.5f),
		end_position =
			start_position +
			arrow_length *
			read_imagef(
				vector_field,
				absolute_clamping_nearest,
				this_pos).xy;

	// Position 1
	vb[base_index] = start_position.x;
	vb[base_index+1] = start_position.y;

	// Color 1 (rgb)
	vb[base_index+2] = 0.0f;
	vb[base_index+3] = 0.0f;
	vb[base_index+4] = 0.0f;

	// Position 2
	vb[base_index+5] = end_position.x;
	vb[base_index+6] = end_position.y;

	// Color 1 (rgb)
	vb[base_index+7] = 1.0f;
	vb[base_index+8] = 1.0f;
	vb[base_index+9] = 1.0f;
}
