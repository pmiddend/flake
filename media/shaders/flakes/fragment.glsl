#version 140

$$$HEADER$$$

out vec4 frag_color;

void
main()
{
	vec4 texture_value =
		texture(
			tex,
			gl_PointCoord);
	frag_color =
		vec4(
			texture_value.xyz,
			texture_value.w * 0.5);
}
