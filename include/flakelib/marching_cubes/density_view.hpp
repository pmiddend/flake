#ifndef FLAKELIB_MARCHING_CUBES_DENSITY_VIEW_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_DENSITY_VIEW_HPP_INCLUDED

#include <flakelib/volume/float_view.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace marching_cubes
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::volume::float_view,
	density_view);
}
}

#endif

