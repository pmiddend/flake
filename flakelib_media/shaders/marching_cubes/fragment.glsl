#version 140

$$$HEADER$$$

in vec3 normal_interp;
in vec3 position_interp;
out vec4 frag_color;

void main()
{
	float
		ambient =
			0.5,
		intensity =
			ambient +
			max(
				0.0,
				dot(
					normal_interp,
					-sun_direction));

	frag_color =
		vec4(
			intensity,
			intensity,
			intensity,
			1.0);
}