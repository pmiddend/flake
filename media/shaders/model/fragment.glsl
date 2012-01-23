#version 140

$$$HEADER$$$

in vec2 texcoord_interp;
in vec3 normal_interp;
in vec3 position_interp;
out vec4 frag_color;

void main()
{
	vec3 light_to_pos =
		light_position - position_interp;

	float dist =
		length(
			light_to_pos);

	vec3 normal = normal_interp;

	float intensity =
		max(
			0.0,
			dot(
				normal,
				light_to_pos / dist));

	frag_color =
		vec4(
			intensity *
			texture(
				primary_texture,
				texcoord_interp).xyz,
			1.0);
}
