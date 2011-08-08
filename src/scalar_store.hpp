#ifndef FLAKE_SCALAR_STORE_HPP_INCLUDED
#define FLAKE_SCALAR_STORE_HPP_INCLUDED

#include "scalar_color_format.hpp"
#include <mizuiro/image/store.hpp>

namespace flake
{
typedef
mizuiro::image::store<flake::scalar_color_format>
scalar_store;
}

#endif
