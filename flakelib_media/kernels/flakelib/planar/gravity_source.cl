#include "planar/positions.cl"

kernel void
apply(
	float const magnitude,
	global float2 *input,
	float const dt)
{
	input[get_global_id(0)] += dt * (float2)(0.0,magnitude);
}
