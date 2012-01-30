kernel void
apply(
	//global float const *boundary,
	float const temperature_strength,
	float const density_strength,
	float const ambient_temperature,
	float const dt,
	global float2 *velocity,
	global float const *temperature,
	global float const *density)
{
	//if(boundary[get_global_id(0)] > 0.2f)
	//	return;

	velocity[get_global_id(0)] +=
		(float2)(
			0.0,
			dt *
			(
				- density_strength * density[get_global_id(0)] +
				temperature_strength * (temperature[get_global_id(0)] - ambient_temperature)));
}
