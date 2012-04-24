#version 140

$$$HEADER$$$

out vec4 frag_color;
in vec2 texcoord_interp;

void
main()
{
	vec4 texture_value =
		texture(
			tex,
			texcoord_interp + gl_PointCoord * vec2(tile_size,1.0));

	frag_color =
		vec4(
			texture_value.xyz,
			texture_value.w * 0.5);
}
