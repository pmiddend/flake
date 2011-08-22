#ifndef FLAKE_CONST_SCALAR_VIEW_HPP_INCLUDED
#define FLAKE_CONST_SCALAR_VIEW_HPP_INCLUDED

#include "scalar_view.hpp"
#include <mizuiro/image/const_view.hpp>

namespace flake
{
typedef
mizuiro::image::const_view<flake::scalar_view>::type
const_scalar_view;
}

#endif
