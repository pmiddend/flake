#ifndef FLAKELIB_MONITOR_CHILD_HPP_INCLUDED
#define FLAKELIB_MONITOR_CHILD_HPP_INCLUDED

#include <flakelib/monitor/optional_projection.hpp>
#include <flakelib/monitor/parent_fwd.hpp>
#include <rucksack/widget/base_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace monitor
{
class child
:
	public
		boost::intrusive::list_base_hook
		<
			boost::intrusive::link_mode<boost::intrusive::auto_unlink>
		>
{
FCPPT_NONCOPYABLE(
	child);
public:
	virtual fcppt::string const
	name() const = 0;

	virtual void
	render(
		monitor::optional_projection const &) = 0;

	virtual void
	update() = 0;

	virtual rucksack::widget::base &
	widget() = 0;

	virtual ~child() = 0;
protected:
	explicit
	child(
		monitor::parent &);

	monitor::parent &parent_;

	monitor::parent &
	parent() const;
};
}
}

#endif
