#version 140

$$$HEADER$$$

void
main()
{
	gl_PointSize =
		100.0f;

	gl_Position =
		mvp *
		vec4(position.xyz,1.0);
}
