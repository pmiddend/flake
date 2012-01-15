#version 140

$$$HEADER$$$

out vec4 frag_color;

void
main()
{
	frag_color =
		texture(
			tex,
			gl_PointCoord);
}
