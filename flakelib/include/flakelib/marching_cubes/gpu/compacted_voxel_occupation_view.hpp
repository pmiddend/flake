#ifndef FLAKELIB_MARCHING_CUBES_GPU_COMPACTED_VOXEL_OCCUPATION_VIEW_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_GPU_COMPACTED_VOXEL_OCCUPATION_VIEW_HPP_INCLUDED

#include <flakelib/volume/uint_view.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace marching_cubes
{
namespace gpu
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::volume::uint_view,
	compacted_voxel_occupation_view);
}
}
}

#endif

