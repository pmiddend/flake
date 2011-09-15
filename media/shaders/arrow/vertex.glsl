#version 140

$$$HEADER$$$

out vec4 color_interp;

void
main()
{
	color_interp = vec4(color,1.0);
	gl_Position = projection * vec4(initial_position + position,0.0,1.0);
}
