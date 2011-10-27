#ifndef FLAKELIB_CL_APPLY_KERNEL_TO_PLANAR_IMAGE_HPP_INCLUDED
#define FLAKELIB_CL_APPLY_KERNEL_TO_PLANAR_IMAGE_HPP_INCLUDED

#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object_fwd.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>


namespace flakelib
{
namespace cl
{
void
apply_kernel_to_planar_image(
	sge::opencl::kernel::object &,
	sge::opencl::command_queue::object &,
	sge::opencl::memory_object::image::planar &);
}
}

#endif
