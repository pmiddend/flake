#ifndef FLAKELIB_PLANAR_PROGRAM_CONTEXT_HPP_INCLUDED
#define FLAKELIB_PLANAR_PROGRAM_CONTEXT_HPP_INCLUDED

#include <flakelib/cl/compiler_flags.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <fcppt/nonassignable.hpp>


namespace flakelib
{
namespace planar
{
class program_context
{
FCPPT_NONASSIGNABLE(
	program_context);
public:
	explicit
	program_context(
		sge::opencl::command_queue::object &,
		flakelib::cl::compiler_flags const &);


	sge::opencl::command_queue::object &
	command_queue() const;

	flakelib::cl::compiler_flags const
	compiler_flags() const;
private:
	sge::opencl::command_queue::object &command_queue_;
	flakelib::cl::compiler_flags const compiler_flags_;
};
}
}

#endif

