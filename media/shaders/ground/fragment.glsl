#version 140

$$$HEADER$$$

in vec2 texcoord_interp;
out vec4 frag_color;

void main()
{
	float distribution =
		texture(
			snow_distribution,
			texcoord_interp).r;

	frag_color =
		vec4(
			distribution) +
		(1.0 - distribution) *
		texture(
			ground_texture,
			10.0 * texcoord_interp);
}
