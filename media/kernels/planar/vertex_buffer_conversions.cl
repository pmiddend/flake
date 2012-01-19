struct __attribute__((packed)) vertex
{
	float2 start_position;
	float2 end_position;
	float2 arrow_tip_1_start;
	float2 arrow_tip_1_end;
	float2 arrow_tip_2_start;
	float2 arrow_tip_2_end;
};

kernel void
to_arrow_vb(
	global struct vertex *vb,
	global float2 const *buffer,
	uint buffer_width,
	float const grid_scale,
	float const arrow_scale)
{
	int2 const this_pos = (int2)(
		get_global_id(0),
		get_global_id(1));

	size_t const base_index =
		this_pos.y * buffer_width + this_pos.x;

	float2 const this_pos_float =
		(float2)(
			(float)this_pos.x,
			(float)this_pos.y);

	float2 const this_arrow = buffer[base_index];
	float2 const start_position = grid_scale * (this_pos_float + 0.5f);
	float2 const end_position = start_position + arrow_scale * (float2)(this_arrow.x,this_arrow.y);
	float2 const first_normal =
		fast_normalize(
			(float2)(
				this_arrow.y,-this_arrow.x));
	float2 const second_normal =
		fast_normalize(
			(float2)(
				-this_arrow.y,this_arrow.x));

	float2 arrow_tip_1_start;
	float2 arrow_tip_1_end;
	float2 arrow_tip_2_start;
	float2 arrow_tip_2_end;

	vb[base_index].start_position = start_position;
	vb[base_index].end_position = end_position;
	vb[base_index].arrow_tip_1_start = end_position;
	vb[base_index].arrow_tip_2_start = end_position;
	vb[base_index].arrow_tip_1_end =
		start_position +
		arrow_scale * this_arrow * 0.8 +
		fast_length(this_arrow) * 0.1 * first_normal;
	vb[base_index].arrow_tip_2_end =
		start_position +
		arrow_scale * this_arrow * 0.8 +
		fast_length(this_arrow) * 0.1 * second_normal;
}

kernel void
scalar_to_texture(
	global float const *input,
	global write_only image2d_t output,
	float const scaling)
{
	int2 const this_pos =
		(int2)(
			get_global_id(0),
			get_global_id(1));

	float const rgb =
		scaling *
		input[get_image_width(output) * this_pos.y + this_pos.x];

	write_imagef(
		output,
		this_pos,
		(float4)(
			rgb,
			rgb,
			rgb,
			1.0f));
}
