#ifndef FLAKELIB_VOLUME_CONVERSION_CL_BUFFER_HPP_INCLUDED
#define FLAKELIB_VOLUME_CONVERSION_CL_BUFFER_HPP_INCLUDED

#include <flakelib/buffer/volume_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
namespace conversion
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::buffer::volume_view<cl_float4>,
	cl_buffer);
}
}
}

#endif
