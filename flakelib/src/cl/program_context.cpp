#include <flakelib/cl/program_context.hpp>
#include <fcppt/log/context_fwd.hpp>


flakelib::cl::program_context::program_context(
	fcppt::log::context &_log_context,
	sge::opencl::command_queue::object &_command_queue,
	flakelib::cl::compiler_flags const &_compiler_flags)
:
	log_context_{
		_log_context
	},
	command_queue_(
		_command_queue),
	compiler_flags_(
		_compiler_flags)
{
}

fcppt::log::context &
flakelib::cl::program_context::log_context() const
{
	return
		log_context_;
}

sge::opencl::command_queue::object &
flakelib::cl::program_context::command_queue() const
{
	return
		command_queue_;
}

flakelib::cl::compiler_flags const
flakelib::cl::program_context::compiler_flags() const
{
	return
		compiler_flags_;
}
