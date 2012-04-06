#ifndef FLAKELIB_CL_PROGRAM_CONTEXT_HPP_INCLUDED
#define FLAKELIB_CL_PROGRAM_CONTEXT_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/cl/compiler_flags.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <fcppt/nonassignable.hpp>


namespace flakelib
{
namespace cl
{
class program_context
{
FCPPT_NONASSIGNABLE(
	program_context);
public:
	FLAKELIB_SYMBOL
	program_context(
		sge::opencl::command_queue::object &,
		flakelib::cl::compiler_flags const &);


	FLAKELIB_SYMBOL
	sge::opencl::command_queue::object &
	command_queue() const;

	FLAKELIB_SYMBOL
	flakelib::cl::compiler_flags const
	compiler_flags() const;
private:
	sge::opencl::command_queue::object &command_queue_;
	flakelib::cl::compiler_flags const compiler_flags_;
};
}
}

#endif

