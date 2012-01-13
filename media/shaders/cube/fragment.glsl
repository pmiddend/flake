#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

// Haupt-Parameter fürs Raycasting
// selbsterklärend...
const float stepsize = 0.5;
// Hauptdiagonale des Würfels ist die längste mögliche Strecke
const int steps = int((sqrt(3.0) * 64)/stepsize);

// höhere Werte ~= höhere Dichte
const float opacity = 25.0;

float
tex_value(
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
			tex,
			vec2(
				(float(left) + x)/512.0,
				(float(top) + y)/512.0)).r;
}

void
main()
{
	vec3 direction = normalize(position_interp - camera);
	// scaling density for uniform cloud data
	float density = stepsize * opacity;

	vec3 position = position_interp;
	float dst = 0.0;

	for(int i = 0; i < steps; i++)
	{
		// FIXME: Interpolate z here!
		float value =
			tex_value(
				position.x,
				position.y,
				int(
					position.z));

		dst +=
			(1.0 - dst) *
			density *
			value;

		position += direction * stepsize;

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
					vec3(64.0,64.0,64.0))))
			break;
		/*
		// ray termination - cube borders
		vec3 temp1 = sign(position);
		vec3 temp2 = sign(1.0 - position);
		if (dot(temp1, temp2) < 3.0)
			break;
			*/
	}

	frag_color = vec4(1.0,1.0,1.0,dst);
	//frag_color = vec4(1.0,1.0,1.0,0.5);
}
