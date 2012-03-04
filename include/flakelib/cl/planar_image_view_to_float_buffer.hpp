#ifndef FLAKELIB_CL_PLANAR_IMAGE_VIEW_TO_FLOAT_BUFFER_HPP_INCLUDED
#define FLAKELIB_CL_PLANAR_IMAGE_VIEW_TO_FLOAT_BUFFER_HPP_INCLUDED

#include <flakelib/buffer/planar_view_fwd.hpp>
#include <flakelib/symbol.hpp>
#include <sge/image2d/view/const_object_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>


namespace flakelib
{
namespace cl
{
FLAKELIB_SYMBOL
void
planar_image_view_to_float_buffer(
	sge::opencl::command_queue::object &,
	sge::image2d::view::const_object const &,
	flakelib::buffer::planar_view<cl_float> const &);
}
}

#endif
