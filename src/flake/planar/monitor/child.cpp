#include <flake/planar/monitor/child.hpp>
#include <flake/planar/monitor/parent.hpp>

flake::planar::monitor::child::~child()
{
	parent_.erase_child(
		*this);
}

flake::planar::monitor::child::child(
	monitor::parent &_parent)
:
	parent_(
		_parent)
{
	parent_.add_child(
		*this);
}

flake::planar::monitor::parent &
flake::planar::monitor::child::parent() const
{
	return parent_;
}
