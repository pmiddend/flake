#ifndef FLAKELIB_CL_PLANAR_IMAGE_VIEW_TO_CL_IMAGE_HPP_INCLUDED
#define FLAKELIB_CL_PLANAR_IMAGE_VIEW_TO_CL_IMAGE_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <sge/image2d/view/const_object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <fcppt/unique_ptr.hpp>


namespace flakelib
{
namespace cl
{
FLAKELIB_SYMBOL
fcppt::unique_ptr<sge::opencl::memory_object::image::planar>
planar_image_view_to_cl_image(
	sge::image2d::view::const_object const &,
	sge::opencl::command_queue::object &);
}
}

#endif
