#ifndef FLAKELIB_CL_PROCESS_PLANAR_BUFFER_HPP_INCLUDED
#define FLAKELIB_CL_PROCESS_PLANAR_BUFFER_HPP_INCLUDED

#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object_fwd.hpp>
#include <sge/opencl/memory_object/dim2.hpp>

namespace flakelib
{
namespace cl
{
void
process_planar_buffer(
	sge::opencl::command_queue::object &,
	sge::opencl::kernel::object &,
	sge::opencl::memory_object::dim2 const &);
}
}

#endif

