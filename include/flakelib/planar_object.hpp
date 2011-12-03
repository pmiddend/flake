#ifndef FLAKELIB_PLANAR_OBJECT_HPP_INCLUDED
#define FLAKELIB_PLANAR_OBJECT_HPP_INCLUDED

#include <flakelib/planar_buffer.hpp>
#include <flakelib/invalid_planar_object.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <fcppt/variant/object_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
typedef
fcppt::variant::object
<
	boost::mpl::vector3
	<
		flakelib::planar_buffer,
		// This cannot be a reference since that's not possible in a
		// variant (and would be too hard to implement)
		sge::opencl::memory_object::image::planar *,
		flakelib::invalid_planar_object
	>
>
planar_object;
}

#endif
