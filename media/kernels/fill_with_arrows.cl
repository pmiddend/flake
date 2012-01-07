#include "positions.cl"

kernel void fill_with_arrows(
	global float4 *buffer,
	global float const *boundary)
{
	int3 const pos =
		(int3)(
			get_global_id(0),
			get_global_id(1),
			get_global_id(2));

	buffer[FLAKE_VOLUME_AT(get_global_size(0),pos)] =
		boundary[FLAKE_VOLUME_AT(get_global_size(0),pos)] > 0.5f
		?
			(float4)(
				1.0f,
				0.0f,
				0.0f,
				1.0f)
		:
			(float4)(
				0.0f);
}
