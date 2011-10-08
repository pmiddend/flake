#include <flakelib/visualization/monitor/base.hpp>
#include <flakelib/visualization/monitor/parent.hpp>

flakelib::visualization::monitor::base::~base()
{
	parent_.erase_child(
		*this);
}

flakelib::visualization::monitor::base::base(
	monitor::parent &_parent)
:
	parent_(
		_parent)
{
	parent_.add_child(
		*this);
}

flakelib::visualization::monitor::parent &
flakelib::visualization::monitor::base::parent() const
{
	return parent_;
}
