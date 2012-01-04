#ifndef FLAKELIB_PLANAR_BOUNDARY_VIEW_HPP_INCLUDED
#define FLAKELIB_PLANAR_BOUNDARY_VIEW_HPP_INCLUDED

#include <sge/image2d/view/const_object.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace planar
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::image2d::view::const_object,
	boundary_view);
}
}

#endif
