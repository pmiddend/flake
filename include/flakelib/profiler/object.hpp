#ifndef FLAKELIB_PROFILER_OBJECT_HPP_INCLUDED
#define FLAKELIB_PROFILER_OBJECT_HPP_INCLUDED

#include <flakelib/profiler/activation.hpp>
#include <flakelib/profiler/call_count.hpp>
#include <flakelib/profiler/child_sequence.hpp>
#include <flakelib/profiler/duration.hpp>
#include <flakelib/profiler/optional_parent.hpp>
#include <flakelib/profiler/scoped_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/string.hpp>
#include <fcppt/chrono/duration_impl.hpp>
#include <fcppt/io/ostream.hpp>


namespace flakelib
{
namespace profiler
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		fcppt::string const &,
		profiler::optional_parent const &,
		profiler::activation::type);

	fcppt::string const &
	name() const;

	profiler::child_sequence const &
	children() const;

	profiler::call_count
	calls() const;

	profiler::duration const &
	total_time() const;

	profiler::activation::type
	activation() const;

	void
	activation(
		profiler::activation::type);

	~object();
private:
	friend class profiler::scoped;

	fcppt::string const name_;
	profiler::child_sequence children_;
	profiler::call_count calls_;
	profiler::duration total_time_;
	profiler::activation::type activation_;

	void
	add_child(
		profiler::object &);

	void
	add_call(
		profiler::duration const &);
};

fcppt::io::ostream &
operator<<(
	fcppt::io::ostream &,
	profiler::object const &);
}
}


#endif
