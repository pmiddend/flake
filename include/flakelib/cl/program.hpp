#ifndef FLAKELIB_CL_PROGRAM_HPP_INCLUDED
#define FLAKELIB_CL_PROGRAM_HPP_INCLUDED

#include <flakelib/cl/compiler_flags.hpp>
#include <flakelib/symbol.hpp>
#include <flakelib/cl/kernel_parameters.hpp>
#include <flakelib/cl/unique_kernel_ptr.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/name.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/unique_ptr_impl.hpp>
#include <fcppt/filesystem/path.hpp>
#include <fcppt/config/external_begin.hpp>
#include <map>
#include <string>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace cl
{
class program
{
FCPPT_NONCOPYABLE(
	program);
public:
	// Taking a command queue instead a the (more correct) context binds
	// the program to a specific command queue. Since we're simulating with
	// just one queue everywhere, that's not a problem.
	FLAKELIB_SYMBOL
	explicit
	program(
		sge::opencl::command_queue::object &,
		fcppt::filesystem::path const &,
		flakelib::cl::compiler_flags const &);

	FLAKELIB_SYMBOL
	cl::unique_kernel_ptr
	create_kernel(
		sge::opencl::kernel::name const &);

	FLAKELIB_SYMBOL
	~program();
private:
	typedef
	std::map<std::string,cl::kernel_parameters>
	kernel_name_to_parameters;

	sge::opencl::command_queue::object &command_queue_;
	sge::opencl::program::object program_;
	kernel_name_to_parameters kernel_name_to_parameters_;

	void
	fill_kernel_name_to_parameters(
		fcppt::filesystem::path const &);
};
}
}

#endif

