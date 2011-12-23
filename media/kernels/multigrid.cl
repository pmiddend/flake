#include "float_handling.cl"
#include "positions.cl"

kernel void
add(
	global float const *p1,
	global float const *p2,
	global float *output)
{
	output[get_global_id(0)] =
		p1[get_global_id(0)] +
		p2[get_global_id(0)];
}

// from = big
// to = small
// We iterate over the _small_ image and gather texels from the _large_ image.
kernel void
downsample(
	global float const *from,
	global float *to,
	int const large_buffer_width)
{
	int2 const currentpos =
		(int2)(
			2 * get_global_id(0),
			2 * get_global_id(1));

	// test
	int const
		current_index =
			FLAKE_AT(large_buffer_width,currentpos),
		left_index =
			FLAKE_LEFT_OF(large_buffer_width,currentpos),
		right_index =
			FLAKE_RIGHT_OF(large_buffer_width,currentpos),
		top_index =
			FLAKE_TOP_OF(large_buffer_width,currentpos),
		bottom_index =
			FLAKE_BOTTOM_OF(large_buffer_width,currentpos),
		lefttop_index =
			FLAKE_LEFT_TOP_OF(large_buffer_width,currentpos),
		righttop_index =
			FLAKE_RIGHT_TOP_OF(large_buffer_width,currentpos),
		leftbottom_index =
			FLAKE_LEFT_BOTTOM_OF(large_buffer_width,currentpos),
		rightbottom_index =
			FLAKE_RIGHT_BOTTOM_OF(large_buffer_width,currentpos);

	flake_real
		center =
			from[current_index],
		left =
			from[left_index],
		right =
			from[right_index],
		top =
			from[top_index],
		bottom =
			from[bottom_index],
		lefttop =
			from[lefttop_index],
		leftbottom =
			from[leftbottom_index],
		rightbottom =
			from[rightbottom_index],
		righttop =
			from[righttop_index];

	flake_real const
		diagonals =
			leftbottom + lefttop + righttop + rightbottom,
		von_neumann =
			left + right + top + bottom,
		output =
			FLAKE_REAL_LIT(1.0)/FLAKE_REAL_LIT(16.0) *
			(diagonals + FLAKE_REAL_LIT(2.0) * von_neumann + FLAKE_REAL_LIT(4.0) * center);

	int2 const small_position =
		currentpos/2;

	to[FLAKE_AT(large_buffer_width/2,small_position)] =
		//center;
		output;
}

// from = small
// to = big
// We iterate over the _big_ image.
kernel void
upsample_(
	global float const *from,
	global float *to,
	int const small_buffer_width)
{
	int2 const position_big =
		(int2)(
			get_global_id(0),
			get_global_id(1));

	int2 const position_small =
		position_big/2;

	flake_real const
		left =
			from[FLAKE_AT(small_buffer_width,position_small)],
		right =
			from[FLAKE_RIGHT_OF(small_buffer_width,position_small)],
		leftbottom =
			from[FLAKE_BOTTOM_OF(small_buffer_width,position_small)],
		rightbottom =
			from[FLAKE_RIGHT_BOTTOM_OF(small_buffer_width,position_small)];

	flake_real const
		fraction_x =
			(position_big.x % 2)/FLAKE_REAL_LIT(2.0),
		fraction_y =
			(position_big.y % 2)/FLAKE_REAL_LIT(2.0);

	to[position_big.y * small_buffer_width * 2 + position_big.x] =
		mix(
			mix(
				left,
				right,
				fraction_x),
			mix(
				leftbottom,
				rightbottom,
				fraction_x),
			fraction_y);

}

kernel void
laplacian_residual(
	global float const *rhs,
	global float const *boundary,
	global float const *from,
	global float *to,
	int const buffer_width,
	float const grid_scale)
{
	int2 const currentpos =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	int const
		current_index =
			FLAKE_AT(buffer_width,currentpos),
		left_index =
			FLAKE_LEFT_OF(buffer_width,currentpos),
		right_index =
			FLAKE_RIGHT_OF(buffer_width,currentpos),
		top_index =
			FLAKE_TOP_OF(buffer_width,currentpos),
		bottom_index =
			FLAKE_BOTTOM_OF(buffer_width,currentpos);

	flake_real const
		center =
			from[current_index],
		left =
			mix(
				from[left_index],
				center,
				boundary[left_index]),
		right =
			mix(
				from[right_index],
				center,
				boundary[right_index]),
		top =
			mix(
				from[top_index],
				center,
				boundary[top_index]),
		bottom =
			mix(
				from[bottom_index],
				center,
				boundary[bottom_index]);

	flake_real const
		laplace =
			native_divide(
				left + right + top + bottom - FLAKE_REAL_LIT(4.0) * center,
				grid_scale * grid_scale),
		rhs_value =
			rhs[current_index];

	to[current_index] =
		rhs_value - laplace;
}
