#ifndef FLAKELIB_PROFILER_OBJECT_HPP_INCLUDED
#define FLAKELIB_PROFILER_OBJECT_HPP_INCLUDED

#include <flakelib/profiler/title.hpp>
#include <flakelib/profiler/is_active.hpp>
#include <flakelib/profiler/call_count.hpp>
#include <flakelib/profiler/child_sequence.hpp>
#include <flakelib/profiler/optional_parent.hpp>
#include <flakelib/profiler/duration.hpp>
#include <flakelib/profiler/parent.hpp>
#include <flakelib/profiler/scoped_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/string.hpp>
#include <fcppt/chrono/duration_impl.hpp>
#include <fcppt/io/ostream.hpp>
#include <fcppt/optional_impl.hpp>


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
		profiler::title const &,
		profiler::parent const &);

	explicit
	object(
		profiler::title const &,
		profiler::is_active const &);

	profiler::title const &
	title() const;

	profiler::child_sequence const &
	children() const;

	profiler::call_count
	calls() const;

	profiler::duration const &
	total_time() const;

	bool
	is_active() const;

	void
	is_active(
		bool);

	~object();
private:
	friend class profiler::scoped;

	profiler::title const title_;
	profiler::optional_parent parent_;
	profiler::child_sequence children_;
	profiler::call_count calls_;
	profiler::duration total_time_;
	bool is_active_;

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
