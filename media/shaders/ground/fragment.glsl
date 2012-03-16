#version 140

$$$HEADER$$$

in vec2 texcoord_interp;
in vec3 position_interp;
out vec4 frag_color;

void main()
{
	float distribution =
		texture(
			snow_distribution,
			texcoord_interp).r;

	// Light stuff begin
	vec3 normal =
		vec3(
			0.0,
			1.0,
			0.0);

	float
		ambient =
			0.5,
		intensity =
			1.0
			/*
			max(
				0.0,
				dot(
					normal,
					-sun_direction))*/;
	// Light stuff end

	// Blend snow and texture together
	vec4 snow_result =
		texture(
			ground_texture,
			10.0 * texcoord_interp) +
		vec4(
			distribution);
		/*
		mix(
			texture(
				ground_texture,
				10.0 * texcoord_interp),
			vec4(
				distribution),
			distribution);
			*/

	// Blend light and snow result
	frag_color =
		vec4(
			intensity * snow_result.xyz,
			1.0);
}
