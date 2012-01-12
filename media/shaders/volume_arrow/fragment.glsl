#version 140

$$$HEADER$$$

in vec4 color_interp;
in float camera_distance_interp;
out vec4 fragment_color;

void
main()
{
	fragment_color =
		vec4(
			color_interp.xyz,
			1.0 - camera_distance_interp / 16.0);
}
