#ifndef FLAKE_PLANAR_CLAMP_INT2_TO_GLOBAL_RECT_HPP_INCLUDED
#define FLAKE_PLANAR_CLAMP_INT2_TO_GLOBAL_RECT_HPP_INCLUDED

int2
flake_planar_clamp_int2_to_global_rect(
	int2 const position)
{
	return
		clamp(
			position,
			(int2)(0),
			(int2)(
				get_global_size(0)-1,
				get_global_size(1)-1));
}

#endif
