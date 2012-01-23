#version 140

$$$HEADER$$$

out vec2 texcoord_interp;
out vec3 normal_interp;
out vec3 position_interp;

void
main()
{
	gl_Position =
		mvp *
		vec4(
			position,
			1.0);

	position_interp =
		world * position;

	normal_interp =
		normal;

	texcoord_interp = texcoord;
}
