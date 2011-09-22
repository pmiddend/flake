#ifndef FLAKE_BOUNDARY_VIEW_HPP_INCLUDED
#define FLAKE_BOUNDARY_VIEW_HPP_INCLUDED

#include <sge/image2d/view/const_object.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flake
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::image2d::view::const_object,
	boundary_view);
}

#endif
