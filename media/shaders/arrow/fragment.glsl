#version 140

$$$HEADER$$$

in vec4 color_interp;
out vec4 fragment_color;

void
main()
{
	fragment_color =
		color_interp;
}
