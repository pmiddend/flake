#ifndef FLAKELIB_PLANAR_OBJECT_SIZE_HPP_INCLUDED
#define FLAKELIB_PLANAR_OBJECT_SIZE_HPP_INCLUDED

#include <flakelib/planar_object.hpp>
#include <sge/opencl/memory_object/dim2.hpp>
#include <fcppt/math/dim/basic_impl.hpp>

namespace flakelib
{
sge::opencl::memory_object::dim2 const
planar_object_size(
	flakelib::planar_object const &);
}

#endif
