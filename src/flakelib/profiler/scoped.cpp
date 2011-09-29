#include <flakelib/profiler/scoped.hpp>
#include <flakelib/profiler/object.hpp>
#include <flakelib/profiler/clock.hpp>
#include <fcppt/chrono/time_point_arithmetic.hpp>

flakelib::profiler::scoped::scoped(
	profiler::object &_object)
:
	object_(
		_object),
	start_(
		profiler::clock::now())
{
}

flakelib::profiler::scoped::~scoped()
{
	object_.add_call(
		profiler::clock::now() - start_);
}
