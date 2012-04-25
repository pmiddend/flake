#version 140

$$$HEADER$$$

out vec3 position_interp;

void
main()
{
	vec3 real_position =
		(position + 1.0)/2.0 * slice_width;

	gl_Position =
		mvp * vec4(real_position,1.0);

	position_interp =
		//vec3(mv * real_position);
		real_position;
}
