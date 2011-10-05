#ifndef FLAKELIB_BUFFER_OR_IMAGE_HPP_INCLUDED
#define FLAKELIB_BUFFER_OR_IMAGE_HPP_INCLUDED

#include <flakelib/planar_buffer.hpp>
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
	boost::mpl::vector2
	<
		flakelib::planar_buffer,
		sge::opencl::memory_object::image::planar *
	>
>
buffer_or_image;
}

#endif
