#version 140

$$$HEADER$$$

out vec4 color_interp;
out float camera_distance_interp;

void
main()
{
	color_interp =
		color;
	camera_distance_interp =
		length(
			vec3(position) - camera_position);
	gl_Position =
		mvp *
		/*(vec4(initial_position,1.0) + */position/*)*/;
}
