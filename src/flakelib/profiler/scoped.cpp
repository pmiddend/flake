#include <flakelib/profiler/clock.hpp>
#include <flakelib/profiler/object.hpp>
#include <flakelib/profiler/scoped.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <fcppt/chrono/time_point_arithmetic.hpp>


flakelib::profiler::scoped::scoped(
	profiler::object &_object,
	sge::opencl::command_queue::object &_command_queue)
:
	command_queue_(
		_command_queue),
	object_(
		_object),
	start_()
{
	if(_object.is_active())
	{
		command_queue_.finish();
		start_ = profiler::clock::now();
	}
}

flakelib::profiler::scoped::~scoped()
{
	if(object_.is_active())
	{
		command_queue_.finish();
		object_.add_call(
			profiler::clock::now() - start_);
	}
}
