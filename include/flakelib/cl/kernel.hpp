#ifndef FLAKELIB_CL_KERNEL_HPP_INCLUDED
#define FLAKELIB_CL_KERNEL_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/cl/kernel_parameters.hpp>
#include <sge/opencl/command_queue/global_dim1.hpp>
#include <sge/opencl/command_queue/global_dim2.hpp>
#include <sge/opencl/command_queue/global_dim3.hpp>
#include <sge/opencl/command_queue/local_dim1.hpp>
#include <sge/opencl/command_queue/local_dim2.hpp>
#include <sge/opencl/command_queue/local_dim3.hpp>
#include <flakelib/cl/uint4.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/name.hpp>
#include <sge/opencl/kernel/numeric_type.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/base_fwd.hpp>
#include <sge/opencl/memory_object/byte_size.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <sge/opencl/program/object_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <set>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace cl
{
class kernel
{
FCPPT_NONCOPYABLE(
	kernel);
public:
	FLAKELIB_SYMBOL
	kernel(
		sge::opencl::program::object &,
		sge::opencl::command_queue::object &,
		sge::opencl::kernel::name const &,
		cl::kernel_parameters const &);

	FLAKELIB_SYMBOL
	void
	numerical_argument(
		std::string const &,
		sge::opencl::kernel::numeric_type const &);

	FLAKELIB_SYMBOL
	void
	buffer_argument(
		std::string const &,
		sge::opencl::memory_object::base &);

	FLAKELIB_SYMBOL
	void
	vector_argument(
		std::string const &,
		flakelib::cl::uint4 const &);

	FLAKELIB_SYMBOL
	void
	planar_image_argument(
		std::string const &,
		sge::opencl::memory_object::image::planar &);

	FLAKELIB_SYMBOL
	void
	raw_memory_argument(
		std::string const &,
		cl_mem);

	FLAKELIB_SYMBOL
	void
	local_buffer_argument(
		std::string const &,
		sge::opencl::memory_object::byte_size const &);

	FLAKELIB_SYMBOL
	void
	enqueue_automatic(
		sge::opencl::command_queue::global_dim1 const &);

	FLAKELIB_SYMBOL
	void
	enqueue_automatic(
		sge::opencl::command_queue::global_dim2 const &);

	FLAKELIB_SYMBOL
	void
	enqueue_automatic(
		sge::opencl::command_queue::global_dim3 const &);

	FLAKELIB_SYMBOL
	void
	enqueue(
		sge::opencl::command_queue::global_dim1 const &,
		sge::opencl::command_queue::local_dim1 const &);

	FLAKELIB_SYMBOL
	void
	enqueue(
		sge::opencl::command_queue::global_dim2 const &,
		sge::opencl::command_queue::local_dim2 const &);

	FLAKELIB_SYMBOL
	void
	enqueue(
		sge::opencl::command_queue::global_dim3 const &,
		sge::opencl::command_queue::local_dim3 const &);

	FLAKELIB_SYMBOL
	sge::opencl::command_queue::object &
	command_queue() const;

	FLAKELIB_SYMBOL
	~kernel();
private:
	typedef
	std::set<std::string>
	unassigned_parameters_set;

	sge::opencl::command_queue::object &command_queue_;
	sge::opencl::kernel::name const name_;
	sge::opencl::kernel::object kernel_;
	cl::kernel_parameters const kernel_parameters_;
	unassigned_parameters_set unassigned_parameters_;

	sge::opencl::kernel::argument_index const
	index_for_name(
		std::string const &);

	void
	check_unassigned_parameters();
};
}
}

#endif

