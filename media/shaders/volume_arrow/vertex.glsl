#version 140

$$$HEADER$$$

out vec4 color_interp;

void
main()
{
	color_interp =
		color;
	gl_Position =
		mvp *
		/*(vec4(initial_position,1.0) + */position/*)*/;
}
