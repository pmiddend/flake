#include <flakelib/visualization/monitor/child.hpp>
#include <flakelib/visualization/monitor/parent.hpp>

flakelib::visualization::monitor::child::~child()
{
	parent_.erase_child(
		*this);
}

flakelib::visualization::monitor::child::child(
	monitor::parent &_parent)
:
	parent_(
		_parent)
{
	parent_.add_child(
		*this);
}

flakelib::visualization::monitor::parent &
flakelib::visualization::monitor::child::parent() const
{
	return parent_;
}
