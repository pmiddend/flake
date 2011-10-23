#ifndef FLAKELIB_PROFILER_SCOPED_HPP_INCLUDED
#define FLAKELIB_PROFILER_SCOPED_HPP_INCLUDED

#include <flakelib/profiler/object_fwd.hpp>
#include <flakelib/profiler/time_point.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/chrono/time_point_impl.hpp>


namespace flakelib
{
namespace profiler
{
class scoped
{
FCPPT_NONCOPYABLE(
	scoped);
public:
	explicit
	scoped(
		profiler::object &,
		sge::opencl::command_queue::object &);

	~scoped();
private:
	sge::opencl::command_queue::object &command_queue_;
	profiler::object &object_;
	profiler::time_point start_;
};
}
}

#endif
