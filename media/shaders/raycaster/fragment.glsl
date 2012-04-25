#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

float
tex_value(
	sampler2D t,
	float x,
	float y,
	int z)
{
	int
		top =
			slice_width * (z / elements_per_row),
		left =
			slice_width * (z % elements_per_row);

	return
		texture(
			t,
			vec2(
				(float(left) + x)/texture_size.x,
				(float(top) + y)/texture_size.y)).r;
}

float
interpolated_tex_value(
	sampler2D t,
	vec3 p)
{
	int discrete_z =
		int(
			p.z);

	float
		first_value =
			tex_value(
				density_texture,
				p.x,
				p.y,
				discrete_z),
		second_value =
			tex_value(
				density_texture,
				p.x,
				p.y,
				min(
					discrete_z+1,
					slice_width-1)),
		interpolated =
			mix(
				first_value,
				second_value,
				fract(
					p.z));

	return
		interpolated;
}

void
main()
{
	const float stepsize = 0.5;

	// Hauptdiagonale des Würfels ist die längste mögliche Strecke
	const int maximum_steps = int(sqrt(3.0*float(slice_width)*float(slice_width))/stepsize);

	// höhere Werte ~= höhere Dichte
	const float opacity = 0.5;

	vec3 direction =
		normalize(
			position_interp - camera_position);

	// scaling density for uniform cloud data
	//float density = stepsize * opacity;

	vec3 position = position_interp;

	float dst = 0.0;

	for(int i = 0; i < maximum_steps; i++)
	{
		float interpolated =
			interpolated_tex_value(
				density_texture,
				position);

		dst +=
			(1.0 - dst) *
	//		density *
			interpolated;

		position +=
			direction * stepsize;

		// ray termination - threshold
		if(dst > 0.90)
			break;

		if(
			any(
				lessThan(
					position,
					vec3(0.0,0.0,0.0))) ||
			any(
				greaterThan(
					position,
					vec3(
						slice_width,
						slice_width,
						slice_width))))
			break;
	}

	frag_color = vec4(1.0,1.0,1.0,dst);
}
