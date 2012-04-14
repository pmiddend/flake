#ifndef FLAKELIB_MARCHING_CUBES_VERTEX_COUNT_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_VERTEX_COUNT_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <sge/renderer/vertex_count.hpp>

namespace flakelib
{
namespace marching_cubes
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::renderer::size_type,
	vertex_count);
}
}

#endif

