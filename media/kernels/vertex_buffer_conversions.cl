sampler_t const absolute_clamping_nearest =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_NEAREST;

struct __attribute__((packed)) vertex
{
	float2 start_position;
	float4 start_color;
	float2 end_position;
	float4 end_color;
};

kernel void
image_to_vb(
	global struct vertex *vb,
	global read_only image2d_t vector_field,
	float const grid_scale,
	float const arrow_scale)
{
	int2 const this_pos = (int2)(
		get_global_id(0),
		get_global_id(1));

	// Each kernel processes two points. Each point has 6 floats stored.
	// Thus, 2 * 5 floats per kernel.
	size_t const base_index =
		this_pos.y * get_image_width(vector_field) + this_pos.x;

	float2 const this_pos_float =
		(float2)(
			(float)this_pos.x,
			(float)this_pos.y);

	vb[base_index].start_position = 
		grid_scale * (this_pos_float + 0.5f);
	vb[base_index].end_position = 
		vb[base_index].start_position +
			arrow_scale *
			read_imagef(
				vector_field,
				absolute_clamping_nearest,
				this_pos).xy;

	vb[base_index].start_color = 
		(float4)(1.0f,0.0f,0.0f,1.0);
	vb[base_index].end_color = 
		(float4)(0.0f,1.0f,0.0f,1.0);
}

kernel void
buffer_to_vb(
	global struct vertex *vb,
	global float2 *buffer,
	uint buffer_width,
	float const grid_scale,
	float const arrow_scale)
{
	int2 const this_pos = (int2)(
		get_global_id(0),
		get_global_id(1));

	// Each kernel processes two points. Each point has 6 floats stored.
	// Thus, 2 * 5 floats per kernel.
	size_t const base_index =
		this_pos.y * buffer_width + this_pos.x;

	float2 const this_pos_float =
		(float2)(
			(float)this_pos.x,
			(float)this_pos.y);

	vb[base_index].start_position = 
		grid_scale * (this_pos_float + 0.5f);
	vb[base_index].end_position = 
		vb[base_index].start_position + arrow_scale * buffer[base_index];

	vb[base_index].start_color = 
		(float4)(1.0f,0.0f,0.0f,1.0f);
	vb[base_index].end_color = 
		(float4)(0.0f,1.0f,0.0f,1.0f);
}

kernel void
image_to_image(
	global read_only image2d_t input,
	global write_only image2d_t output,
	float const scaling)
{
	int2 const this_pos =
		(int2)(
			get_global_id(0),
			get_global_id(1));

	float4 const current_pixel =
		read_imagef(
			input,
			absolute_clamping_nearest,
			this_pos);

	write_imagef(
		output,
		this_pos,
//		scaling * current_pixel);
		(float4)(scaling * current_pixel.xyz,1.0f));
}

kernel void
buffer_to_image(
	global float const *input,
	global write_only image2d_t output,
	float const scaling)
{
	int2 const this_pos =
		(int2)(
			get_global_id(0),
			get_global_id(1));

	write_imagef(
		output,
		this_pos,
		(float4)(
			scaling * input[get_image_width(output) * this_pos.y + this_pos.x]));
}
