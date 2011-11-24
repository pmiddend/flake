#ifndef FLAKELIB_SIMULATION_SCALAR_IMAGE_CACHE_HPP_INCLUDED
#define FLAKELIB_SIMULATION_SCALAR_IMAGE_CACHE_HPP_INCLUDED

#include <flakelib/planar_pool/object_fwd.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace simulation
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::planar_pool::object &,
	scalar_image_cache);
}
}

#endif
