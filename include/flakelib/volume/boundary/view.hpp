#ifndef FLAKELIB_VOLUME_BOUNDARY_VIEW_HPP_INCLUDED
#define FLAKELIB_VOLUME_BOUNDARY_VIEW_HPP_INCLUDED

#include <flakelib/buffer/volume_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
namespace boundary
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::buffer::volume_view<cl_float>,
	view);
}
}
}

#endif
