bool
flake_boundary_is_solid(
	global float const *boundary,
	size_t const index)
{
	return
		boundary[index] > 0.5f;
}
