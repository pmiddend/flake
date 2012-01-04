#include <flakelib/planar/monitor/child.hpp>
#include <flakelib/planar/monitor/parent.hpp>

flakelib::planar::monitor::child::~child()
{
	parent_.erase_child(
		*this);
}

flakelib::planar::monitor::child::child(
	monitor::parent &_parent)
:
	parent_(
		_parent)
{
	parent_.add_child(
		*this);
}

flakelib::planar::monitor::parent &
flakelib::planar::monitor::child::parent() const
{
	return parent_;
}
