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

	vec3 light_to_pos =
		light_position - position_interp;

	float dist =
		length(
			light_to_pos);

	vec3 normal = vec3(0.0,1.0,0.0);

	float intensity =
		max(
			0.0,
			dot(
				normal,
				light_to_pos / dist));

	vec4 snow_result =
		vec4(
			distribution) +
		(1.0 - distribution) *
		texture(
			ground_texture,
			10.0 * texcoord_interp);

	frag_color =
		vec4(
			intensity * snow_result.xyz,
			1.0);
}
