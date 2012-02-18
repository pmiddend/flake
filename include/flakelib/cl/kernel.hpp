#ifndef FLAKELIB_CL_KERNEL_HPP_INCLUDED
#define FLAKELIB_CL_KERNEL_HPP_INCLUDED

#include <sge/opencl/program/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/name.hpp>
#include <sge/opencl/kernel/numeric_type.hpp>
#include <sge/opencl/memory_object/base_fwd.hpp>
#include <sge/opencl/memory_object/dim1.hpp>
#include <sge/opencl/memory_object/dim2.hpp>
#include <sge/opencl/memory_object/dim3.hpp>
#include <sge/opencl/kernel/numeric_type.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <flakelib/cl/kernel_parameters.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace cl
{
class kernel
{
FCPPT_NONCOPYABLE(
	kernel);
public:
	explicit
	kernel(
		sge::opencl::program::object &,
		sge::opencl::command_queue::object &,
		sge::opencl::kernel::name const &,
		cl::kernel_parameters const &);

	void
	numerical_argument(
		std::string const &,
		sge::opencl::kernel::numeric_type const &);

	void
	buffer_argument(
		std::string const &,
		sge::opencl::memory_object::base &);

	void
	enqueue_automatic(
		sge::opencl::memory_object::dim1 const &);

	void
	enqueue_automatic(
		sge::opencl::memory_object::dim2 const &);

	void
	enqueue_automatic(
		sge::opencl::memory_object::dim3 const &);

	~kernel();
private:
	sge::opencl::command_queue::object &command_queue_;
	sge::opencl::kernel::object kernel_;
	cl::kernel_parameters const kernel_parameters_;

	sge::opencl::kernel::argument_index const
	index_for_name(
		std::string const &);
};
}
}

#endif

