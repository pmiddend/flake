#ifndef FLAKELIB_VOLUME_VELOCITY_BUFFER_VIEW_HPP_INCLUDED
#define FLAKELIB_VOLUME_VELOCITY_BUFFER_VIEW_HPP_INCLUDED

#include <flakelib/volume/float4_view.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
FCPPT_MAKE_STRONG_TYPEDEF(
	volume::float4_view,
	velocity_buffer_view);
}
}

#endif

