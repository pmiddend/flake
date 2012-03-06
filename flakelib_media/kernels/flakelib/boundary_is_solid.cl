#ifndef FLAKELIB_BOUNDARY_IS_SOLID_CL_INCLUDED
#define FLAKELIB_BOUNDARY_IS_SOLID_CL_INCLUDED

bool
flakelib_boundary_is_solid(
	global float const *boundary,
	size_t const index)
{
	return
		boundary[index] > 0.5f;
}

#endif
