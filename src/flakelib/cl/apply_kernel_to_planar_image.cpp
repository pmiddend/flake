#include <flakelib/cl/apply_kernel_to_planar_image.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/memory_object/size_type.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <fcppt/assign/make_array.hpp>


void
flakelib::cl::apply_kernel_to_planar_image(
	sge::opencl::kernel::object &_kernel,
	sge::opencl::command_queue::object &_command_queue,
	sge::opencl::memory_object::image::planar &_image)
{
	sge::opencl::command_queue::enqueue_kernel(
		_command_queue,
		_kernel,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_image.size()[0])
			(_image.size()[1]).container(),
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(8)
			(8).container());
}
