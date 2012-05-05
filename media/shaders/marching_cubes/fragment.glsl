#version 140

$$$HEADER$$$

in vec3 normal_interp;
in vec3 position_interp;
out vec4 frag_color;

void main()
{
	float
		ambient =
			0.5,
		intensity =
			ambient +
			max(
				0.0,
				dot(
					-normal_interp,
					-sun_direction));

	float texture_edge_length =
		float(
			textureSize(
				snow_volume_texture,
				0).x);

	vec3 normalized_position =
		position_interp / texture_edge_length;

	vec4 snow_value =
		texture(
			snow_volume_texture,
			texture_repeats * normalized_position);

	frag_color =
		intensity *
		vec4(
			snow_value.xyz,
			1.0);
}
