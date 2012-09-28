#ifndef FLAKE_PLANAR_MONITOR_CHILD_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_CHILD_HPP_INCLUDED

#include <flake/planar/monitor/optional_projection.hpp>
#include <flake/planar/monitor/parent_fwd.hpp>
#include <sge/renderer/context/ffp_fwd.hpp>
#include <sge/rucksack/widget/base_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/preprocessor/disable_gcc_warning.hpp>
#include <fcppt/preprocessor/pop_warning.hpp>
#include <fcppt/preprocessor/push_warning.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace planar
{
namespace monitor
{
FCPPT_PP_PUSH_WARNING
FCPPT_PP_DISABLE_GCC_WARNING(-Weffc++)

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
		sge::renderer::context::ffp &,
		monitor::optional_projection const &) = 0;

	virtual void
	update() = 0;

	virtual sge::rucksack::widget::base &
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

FCPPT_PP_POP_WARNING
}
}
}

#endif
