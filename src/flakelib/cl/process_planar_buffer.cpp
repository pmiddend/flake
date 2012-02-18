#include <flakelib/cl/process_planar_buffer.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/math/dim/basic_impl.hpp>

void
flakelib::cl::process_planar_buffer(
	sge::opencl::command_queue::object &_command_queue,
	sge::opencl::kernel::object &_kernel,
	sge::opencl::memory_object::dim2 const &_size)
{
	sge::opencl::command_queue::enqueue_kernel(
		_command_queue,
		_kernel,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_size[0])
			(_size[1]).container());
}
