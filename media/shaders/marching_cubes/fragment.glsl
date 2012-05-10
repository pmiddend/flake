#version 140

$$$HEADER$$$

in vec3 normal_interp;
in vec3 position_interp;
out vec4 frag_color;

void main()
{
	// NOTE: Is already normalized.
	vec3 normal_vector =
		-normal_interp;

	vec3
		blend_weights =
			(abs(normal_vector) - 0.2) * 7.0;

	blend_weights =
		max(
			blend_weights,
			0.0);

	float blend_sum =
		blend_weights.x + blend_weights.y + blend_weights.z;

	blend_weights /=
		vec3(
			blend_sum,
			blend_sum,
			blend_sum);

	const float repeats =
		1.0/4.0;

	vec2
		coord1 =
			position_interp.yz * repeats,
		coord2 =
			position_interp.zx * repeats,
		coord3 =
			position_interp.xy * repeats;

	vec4
		col1 =
			texture(
				snow_rough,
				coord1),
		col2 =
			texture(
				snow_calm,
				coord2),
		col3 =
			texture(
				snow_rough,
				coord3);

	vec4 blended_color =
		col1 * blend_weights.x +
		col2 * blend_weights.y +
		col3 * blend_weights.z;

	frag_color =
		blended_color;
	/*
	float
		ambient =
			0.5,
		intensity =
			ambient +
			max(
				0.0,
				dot(
					normal_vector,
					-sun_direction));
					*/

	/*
	vec4 snow_value =
		texture(
			snow_volume_texture,
			texture_repeats * normalized_position);

	frag_color =
		intensity *
		vec4(
			snow_value.xyz,
			1.0);
			*/
}
