#version 140

$$$HEADER$$$

out vec3 position_interp;
out vec2 texcoord_interp;

void
main()
{
	gl_Position =
		mvp *
		vec4(
			position,
			1.0);

	position_interp =
		position;

	texcoord_interp =
		texcoord;
}
