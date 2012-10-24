#ifndef FLAKELIB_VOLUME_BOUNDARY_BUFFER_VIEW_HPP_INCLUDED
#define FLAKELIB_VOLUME_BOUNDARY_BUFFER_VIEW_HPP_INCLUDED

#include <flakelib/volume/float_view.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
FCPPT_MAKE_STRONG_TYPEDEF(
	volume::float_view,
	boundary_buffer_view);
}
}

#endif

