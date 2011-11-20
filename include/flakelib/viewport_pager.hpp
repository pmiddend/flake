#ifndef FLAKELIB_VIEWPORT_PAGER_HPP_INCLUDED
#define FLAKELIB_VIEWPORT_PAGER_HPP_INCLUDED

#include <rucksack/widget/base.hpp>
#include <rucksack/widget/base.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/viewport/manager_fwd.hpp>
#include <fcppt/signal/scoped_connection.hpp>


namespace flakelib
{
class viewport_pager
:
	public rucksack::widget::base
{
FCPPT_NONCOPYABLE(
	viewport_pager);
public:
	explicit
	viewport_pager(
		sge::viewport::manager &,
		sge::renderer::device &);

	void
	size(
		rucksack::dim const &);

	void
	position(
		rucksack::vector const &);

	rucksack::dim const
	size() const;

	rucksack::vector const
	position() const;

	rucksack::axis_policy2 const
	axis_policy() const;

	void
	relayout();

	void
	child(
		rucksack::widget::base &);

	~viewport_pager();
private:
	sge::renderer::device &renderer_;
	fcppt::signal::scoped_connection viewport_connection_;
	rucksack::widget::base *child_;

	void
	manage_callback();

	void
	child_destroyed(
		rucksack::widget::base &);
};
}

#endif
