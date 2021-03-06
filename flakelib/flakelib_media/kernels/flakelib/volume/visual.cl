#if 0
#include "volume/positions.cl"

kernel void
volume_image_to_planar_texture(
	/* 0 */global float const *volume_image,
	/* 1 */global write_only image2d_t planar_texture,
	/* 2 */int const slice_edge_size,
	/* 3 */int const elements_per_row)
{
	int3 const volume_pos =
		(int3)(
			get_global_id(0),
			get_global_id(1),
			get_global_id(2));

	int2 const planar_pos =
		slice_edge_size *
		(int2)(
			volume_pos.z % elements_per_row,
			volume_pos.z / elements_per_row) +
		volume_pos.xy;

	write_imagef(
		planar_texture,
		planar_pos,
		(float4)(
			volume_image[FLAKE_VOLUME_AT(slice_edge_size,volume_pos)]));
}
#endif
