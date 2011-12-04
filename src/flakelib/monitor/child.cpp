#include <flakelib/monitor/child.hpp>
#include <flakelib/monitor/parent.hpp>

flakelib::monitor::child::~child()
{
	parent_.erase_child(
		*this);
}

flakelib::monitor::child::child(
	monitor::parent &_parent)
:
	parent_(
		_parent)
{
	parent_.add_child(
		*this);
}

flakelib::monitor::parent &
flakelib::monitor::child::parent() const
{
	return parent_;
}
