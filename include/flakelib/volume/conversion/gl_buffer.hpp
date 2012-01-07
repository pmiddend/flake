#ifndef FLAKELIB_VOLUME_CONVERSION_GL_BUFFER_HPP_INCLUDED
#define FLAKELIB_VOLUME_CONVERSION_GL_BUFFER_HPP_INCLUDED

#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
namespace conversion
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::buffer &,
	gl_buffer);
}
}
}

#endif
