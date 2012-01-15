#version 140

$$$HEADER$$$

void
main()
{
	gl_PointSize =
		point_size;

	gl_Position =
		mvp * vec4(position.xyz,1.0);
}
