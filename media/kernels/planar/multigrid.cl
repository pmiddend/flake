#include "planar/positions.cl"

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
			FLAKE_PLANAR_AT(large_buffer_width,currentpos),
		left_index =
			FLAKE_PLANAR_LEFT_OF(large_buffer_width,currentpos),
		right_index =
			FLAKE_PLANAR_RIGHT_OF(large_buffer_width,currentpos),
		top_index =
			FLAKE_PLANAR_TOP_OF(large_buffer_width,currentpos),
		bottom_index =
			FLAKE_PLANAR_BOTTOM_OF(large_buffer_width,currentpos),
		lefttop_index =
			FLAKE_PLANAR_LEFT_TOP_OF(large_buffer_width,currentpos),
		righttop_index =
			FLAKE_PLANAR_RIGHT_TOP_OF(large_buffer_width,currentpos),
		leftbottom_index =
			FLAKE_PLANAR_LEFT_BOTTOM_OF(large_buffer_width,currentpos),
		rightbottom_index =
			FLAKE_PLANAR_RIGHT_BOTTOM_OF(large_buffer_width,currentpos);

	float
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

	float const
		diagonals =
			leftbottom + lefttop + righttop + rightbottom,
		von_neumann =
			left + right + top + bottom,
		output =
			1.0f/16.0f *
			(diagonals + 2.0f * von_neumann + 4.0f * center);

	int2 const small_position =
		currentpos/2;

	to[FLAKE_PLANAR_AT(large_buffer_width/2,small_position)] =
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

	float const
		left =
			from[FLAKE_PLANAR_AT(small_buffer_width,position_small)],
		right =
			from[FLAKE_PLANAR_RIGHT_OF(small_buffer_width,position_small)],
		leftbottom =
			from[FLAKE_PLANAR_BOTTOM_OF(small_buffer_width,position_small)],
		rightbottom =
			from[FLAKE_PLANAR_RIGHT_BOTTOM_OF(small_buffer_width,position_small)];

	float const
		fraction_x =
			(position_big.x % 2)/2.0f,
		fraction_y =
			(position_big.y % 2)/2.0f;

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
			FLAKE_PLANAR_AT(buffer_width,currentpos),
		left_index =
			FLAKE_PLANAR_LEFT_OF(buffer_width,currentpos),
		right_index =
			FLAKE_PLANAR_RIGHT_OF(buffer_width,currentpos),
		top_index =
			FLAKE_PLANAR_TOP_OF(buffer_width,currentpos),
		bottom_index =
			FLAKE_PLANAR_BOTTOM_OF(buffer_width,currentpos);

	float const
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

	float const
		laplace =
			native_divide(
				left + right + top + bottom - 4.0f * center,
				grid_scale * grid_scale),
		rhs_value =
			rhs[current_index];

	to[current_index] =
		rhs_value - laplace;
}
