#include <flakelib/program/context.hpp>

flakelib::program::context::context(
	flakelib::program::build_options const &_build_options,
	sge::opencl::command_queue::object &_command_queue)
:
	build_options_(
		_build_options),
	command_queue_(
		_command_queue)
{
}

flakelib::program::build_options const &
flakelib::program::context::build_options() const
{
	return 
		build_options_;
}

sge::opencl::command_queue::object &
flakelib::program::context::command_queue() const
{
	return
		command_queue_;
}

flakelib::program::context::~context()
{
}
