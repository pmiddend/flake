#include <flakelib/profiler/scoped.hpp>
#include <flakelib/profiler/object.hpp>
#include <flakelib/profiler/clock.hpp>
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
	if(_object.activation() == profiler::activation::enabled)
	{
		command_queue_.finish();
		start_ = profiler::clock::now();
	}
}

flakelib::profiler::scoped::~scoped()
{
	if(object_.activation() == profiler::activation::enabled)
	{
		command_queue_.finish();
		object_.add_call(
			profiler::clock::now() - start_);
	}
}
