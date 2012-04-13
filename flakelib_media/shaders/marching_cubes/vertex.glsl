#version 140

$$$HEADER$$$

out vec3 normal_interp;
out vec3 position_interp;

void
main()
{
	gl_Position =
		mvp *
		vec4(
			position.xyz,
			1.0);

	position_interp =
		vec3(
			world *
			vec4(
				position.xyz,
				1.0));

	normal_interp =
		normalize(
			normal.xyz);
}