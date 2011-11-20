#include <flakelib/viewport_pager.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/onscreen_target.hpp>
#include <sge/viewport/manager.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/structure_cast.hpp>
#include <fcppt/preprocessor/disable_vc_warning.hpp>
#include <fcppt/preprocessor/pop_warning.hpp>
#include <fcppt/preprocessor/push_warning.hpp>
#include <fcppt/tr1/functional.hpp>
#include <fcppt/config/external_begin.hpp>
#include <limits>
#include <fcppt/config/external_end.hpp>


FCPPT_PP_PUSH_WARNING
FCPPT_PP_DISABLE_VC_WARNING(4355)
flakelib::viewport_pager::viewport_pager(
	sge::viewport::manager &_viewport,
	sge::renderer::device &_renderer)
:
	rucksack::widget::base(
		rucksack::widget::optional_parent()),
	renderer_(
		_renderer),
	viewport_connection_(
		_viewport.manage_callback(
			std::tr1::bind(
				&viewport_pager::manage_callback,
				this))),
	child_(
		0)
{
}
FCPPT_PP_POP_WARNING

void
flakelib::viewport_pager::size(
	rucksack::dim const &_size)
{
	renderer_.onscreen_target().viewport(
		sge::renderer::viewport(
			sge::renderer::viewport::value_type(
				renderer_.onscreen_target().viewport().get().pos(),
				fcppt::math::dim::structure_cast<sge::renderer::viewport::value_type::dim>(
					_size))));
}

void
flakelib::viewport_pager::position(
	rucksack::vector const &_pos)
{
	renderer_.onscreen_target().viewport(
		sge::renderer::viewport(
			sge::renderer::viewport::value_type(
				fcppt::math::vector::structure_cast<sge::renderer::viewport::value_type::vector>(
					_pos),
				renderer_.onscreen_target().viewport().get().size())));
}

rucksack::dim const
flakelib::viewport_pager::size() const
{
	return
		rucksack::dim(
			static_cast<rucksack::scalar>(
				renderer_.onscreen_target().viewport().get().size().w()),
			std::numeric_limits<rucksack::scalar>::max());
}

rucksack::vector const
flakelib::viewport_pager::position() const
{
	return
		fcppt::math::vector::structure_cast<rucksack::vector>(
			renderer_.onscreen_target().viewport().get().pos());
}

rucksack::axis_policy2 const
flakelib::viewport_pager::axis_policy() const
{
	// A dummy for now, until there's a use case
	return
		rucksack::axis_policy2(
			rucksack::axis_policy(
				rucksack::minimum_size(),
				rucksack::preferred_size(),
				rucksack::is_expanding()),
			rucksack::axis_policy(
				rucksack::minimum_size(),
				rucksack::preferred_size(),
				rucksack::is_expanding()),
			rucksack::aspect(
				1,
				1));
}

void
flakelib::viewport_pager::relayout()
{
	if(child_)
		child_->relayout();
}

void
flakelib::viewport_pager::child(
	rucksack::widget::base &_child)
{
	if(child_)
		child_->parent(
			rucksack::widget::optional_parent());

	child_ = &_child;
	child_->parent(
		rucksack::widget::optional_parent(
			*this));
}


flakelib::viewport_pager::~viewport_pager()
{
	if(child_)
		child_->parent(
			rucksack::widget::optional_parent());
}

void
flakelib::viewport_pager::manage_callback()
{
	rucksack::dim const this_size =
		this->size();

	if(!child_)
		return;

	child_->position(
		rucksack::vector::null());
	child_->size(
		rucksack::dim(
			this_size.w(),
			std::numeric_limits<rucksack::scalar>::max()));
	child_->relayout();
}

void
flakelib::viewport_pager::child_destroyed(
	rucksack::widget::base &_child)
{
	FCPPT_ASSERT_PRE(
		&_child == child_);

	child_ =
		0;
}
