#include "positions.cl"

kernel void add_sphere(
	global float *boundary,
	int const x,
	int const y,
	int const z,
	int const r)
{
	int3 const integral_pos =
		(int3)(
			get_global_id(0),
			get_global_id(1),
			get_global_id(2));

	boundary[FLAKE_VOLUME_AT(get_global_size(0),integral_pos)] =
		fast_distance(convert_float3(integral_pos),(float3)(x,y,z)) <= ((float)r)
		?
			1.0f
		:
			boundary[FLAKE_VOLUME_AT(get_global_size(0),integral_pos)];
}

kernel void add_cube(
	global float *boundary,
	int const x,
	int const y,
	int const z,
	int const w)
{
	int3 const integral_pos =
		(int3)(
			get_global_id(0),
			get_global_id(1),
			get_global_id(2));

	boundary[FLAKE_VOLUME_AT(get_global_size(0),integral_pos)] =
		integral_pos.x >= x && integral_pos.x <= (x+w) &&
		integral_pos.y >= y && integral_pos.y <= (y+w) &&
		integral_pos.z >= z && integral_pos.z <= (z+w)
		?
			1.0f
		:
			boundary[FLAKE_VOLUME_AT(get_global_size(0),integral_pos)];
}
