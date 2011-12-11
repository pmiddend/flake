#include "float_handling.cl"
#include "positions.cl"

sampler_t const absolute_clamping_nearest =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_NEAREST;

sampler_t const absolute_clamping_linear =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_LINEAR;

kernel void
add(
	global read_only image2d_t p1,
	global read_only image2d_t p2,
	global write_only image2d_t output)
{
	int2 const position =
		(int2)(
			get_global_id(0),
			get_global_id(1));

	FLAKE_WRITE_IMAGE_FUNCTION(
		output,
		position,
		FLAKE_READ_IMAGE_FUNCTION(
			p1,
			absolute_clamping_nearest,
			position) +
		FLAKE_READ_IMAGE_FUNCTION(
			p2,
			absolute_clamping_nearest,
			position));
}

// from = big
// to = small
// We iterate over the _small_ image and gather texels from the _large_ image.
kernel void
downsample(
	global read_only image2d_t from,
	global write_only image2d_t to)
{
	int2 const position =
		(int2)(
			2 * get_global_id(0),
			2 * get_global_id(1));

	flake_real
		center =
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position).x,
		left =
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position + pos_left).x,
		right =
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position + pos_right).x,
		top =
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom =
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position + pos_bottom).x,
		lefttop =
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position + pos_top + pos_left).x,
		leftbottom =
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position + pos_bottom + pos_left).x,
		rightbottom =
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position + pos_bottom + pos_right).x,
		righttop =
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position + pos_top + pos_right).x;

	flake_real const
		diagonals =
			leftbottom + lefttop + righttop + rightbottom,
		von_neumann =
			left + right + top + bottom,
		output =
			FLAKE_REAL_LIT(1.0)/FLAKE_REAL_LIT(16.0) * (diagonals + FLAKE_REAL_LIT(2.0) * von_neumann + FLAKE_REAL_LIT(4.0) * center);

	FLAKE_WRITE_IMAGE_FUNCTION(
		to,
		// position is the "bigger" position
		position / 2,
		(flake_real4)(
			output,
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0)));
}

// from = small
// to = big
// We iterate over the _big_ image.
kernel void
upsample_(
	global read_only image2d_t from,
	global write_only image2d_t to)
{
	int2 const position_in_big_image =
		(int2)(
			get_global_id(0),
			get_global_id(1));

	FLAKE_WRITE_IMAGE_FUNCTION(
		to,
		position_in_big_image,
		FLAKE_READ_IMAGE_FUNCTION(
			from,
			absolute_clamping_linear,
			FLAKE_CONVERT_REAL2(get_image_dim(from)) *
			FLAKE_CONVERT_REAL2(position_in_big_image) /
			FLAKE_CONVERT_REAL2(get_image_dim(to)) + (flake_real2)(FLAKE_REAL_LIT(0.5))));
}



kernel void
laplacian_residual(
	global read_only image2d_t rhs,
	global read_only image2d_t boundary,
	global read_only image2d_t from,
	global write_only image2d_t to,
	float const grid_scale)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	flake_real const
		left_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_left).x,
		right_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_right).x,
		top_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_top).x,
		bottom_boundary =
			FLAKE_READ_IMAGE_FUNCTION(
				boundary,
				absolute_clamping_nearest,
				position + pos_bottom).x;

	flake_real
		center =
			FLAKE_READ_IMAGE_FUNCTION(
				from,
				absolute_clamping_nearest,
				position).x,
		left =
			mix(
				FLAKE_READ_IMAGE_FUNCTION(
					from,
					absolute_clamping_nearest,
					position + pos_left).x,
				center,
				left_boundary),
		right =
			mix(
				FLAKE_READ_IMAGE_FUNCTION(
					from,
					absolute_clamping_nearest,
					position + pos_right).x,
				center,
				right_boundary),
		top =
			mix(
				FLAKE_READ_IMAGE_FUNCTION(
					from,
					absolute_clamping_nearest,
					position + pos_top).x,
				center,
				top_boundary),
		bottom =
			mix(
				FLAKE_READ_IMAGE_FUNCTION(
					from,
					absolute_clamping_nearest,
					position + pos_bottom).x,
				center,
				bottom_boundary);

	flake_real const
		laplace =
			native_divide(left + right + top + bottom - FLAKE_REAL_LIT(4.0) * center,grid_scale * grid_scale),
		rhs_value =
			FLAKE_READ_IMAGE_FUNCTION(
				rhs,
				absolute_clamping_nearest,
				position).x;

	FLAKE_WRITE_IMAGE_FUNCTION(
		to,
		position,
		(flake_real4)(
			rhs_value - laplace,
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0),
			FLAKE_REAL_LIT(0.0)));
}
