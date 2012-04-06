#ifndef FLAKELIB_VOLUME_CONVERSION_ORIGIN_HPP_INCLUDED
#define FLAKELIB_VOLUME_CONVERSION_ORIGIN_HPP_INCLUDED

#include <sge/renderer/vector3.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
namespace conversion
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::renderer::vector3,
	origin);
}
}
}

#endif

