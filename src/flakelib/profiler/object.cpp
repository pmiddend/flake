#include <flakelib/profiler/object.hpp>
#include <fcppt/chrono/duration_arithmetic.hpp>
#include <fcppt/chrono/duration_cast.hpp>
#include <fcppt/chrono/microseconds.hpp>
#include <fcppt/config/external_begin.hpp>
#include <iomanip>
#include <algorithm>
#include <fcppt/config/external_end.hpp>

namespace
{
void
print(
	fcppt::io::ostream &s,
	flakelib::profiler::object const &o,
	fcppt::string::size_type const tabs,
	fcppt::string::size_type const spaces)
{
	s 
		<< fcppt::string(tabs,FCPPT_TEXT('\t'))
		<< std::left
		<< std::setw(static_cast<int>(spaces))
		<< o.name()
		<< FCPPT_TEXT(" | ") 
		<< std::setw(10)
		<< o.calls() << FCPPT_TEXT(" calls");
	
	if(o.calls())
		s
			<< FCPPT_TEXT(", ")
			<< std::right
			<< std::setw(10)
			<< fcppt::chrono::duration_cast<fcppt::chrono::microseconds>(o.total_time() / o.calls()).count()
			<< FCPPT_TEXT(" microseconds per call");
	
	s << FCPPT_TEXT("\n");

	fcppt::string::size_type max_child_size = 
		0;

	for(
		flakelib::profiler::child_sequence::const_iterator child = 
			o.children().begin();
		child != o.children().end();
		++child)
		max_child_size = 
			std::max(
				max_child_size,
				child->name().length());
	
	if(!o.children().empty())
		s << FCPPT_TEXT("↘") << FCPPT_TEXT("\n");
	
	for(
		flakelib::profiler::child_sequence::const_iterator child = 
			o.children().begin();
		child != o.children().end();
		++child)
		::print(
			s,
			*child,
			tabs+1u,
			max_child_size);
}
}

flakelib::profiler::object::object(
	fcppt::string const &_name,
	profiler::optional_parent const &_parent)
:
	name_(
		_name),
	children_(),
	calls_(
		0),
	total_time_(
		0)
{
	if(_parent)
		_parent->add_child(
			*this);
}

fcppt::string const &
flakelib::profiler::object::name() const
{
	return name_;
}

flakelib::profiler::child_sequence const &
flakelib::profiler::object::children() const
{
	return children_;
}

flakelib::profiler::call_count
flakelib::profiler::object::calls() const
{
	return calls_;
}

flakelib::profiler::duration const &
flakelib::profiler::object::total_time() const
{
	return total_time_;
}

flakelib::profiler::object::~object()
{
}

void
flakelib::profiler::object::add_child(
	profiler::object &_child)
{
	children_.push_back(
		&_child);
}

void
flakelib::profiler::object::add_call(
	profiler::duration const &d)
{
	calls_++;
	total_time_ += d;
}

fcppt::io::ostream &
flakelib::profiler::operator<<(
	fcppt::io::ostream &s,
	profiler::object const &o)
{
	::print(
		s,
		o,
		0u,
		0u);
	
	return s;
}
