#ifndef FLAKE_VOLUME_FLAKES_POSITION_VIEW_HPP_INCLUDED
#define FLAKE_VOLUME_FLAKES_POSITION_VIEW_HPP_INCLUDED

#include <flakelib/buffer/linear_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flake
{
namespace volume
{
namespace flakes
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::buffer::linear_view<cl_float4>,
	position_view);
}
}
}

#endif

