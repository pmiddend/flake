#ifndef FLAKELIB_MARCHING_CUBES_SUMMED_VERTICES_FOR_VOXEL_VIEW_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_SUMMED_VERTICES_FOR_VOXEL_VIEW_HPP_INCLUDED

#include <flakelib/volume/uint_view.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace marching_cubes
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::volume::uint_view,
	summed_vertices_for_voxel_view);
}
}

#endif

