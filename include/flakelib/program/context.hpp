#ifndef FLAKELIB_PROGRAM_CONTEXT_HPP_INCLUDED
#define FLAKELIB_PROGRAM_CONTEXT_HPP_INCLUDED

#include <sge/opencl/command_queue/object_fwd.hpp>
#include <flakelib/program/build_options.hpp>
#include <fcppt/nonassignable.hpp>

namespace flakelib
{
namespace program
{
class context
{
FCPPT_NONASSIGNABLE(
	context);
public:
	explicit
	context(
		flakelib::program::build_options const &,
		sge::opencl::command_queue::object &);

	flakelib::program::build_options const &
	build_options() const;

	sge::opencl::command_queue::object &
	command_queue() const;

	~context();
private:
	flakelib::program::build_options const &_build_options;
	sge::opencl::command_queue::object &_command_queue;
};
}
}

#endif

