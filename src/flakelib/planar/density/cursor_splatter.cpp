#include <flakelib/media_path_from_string.hpp>
#include <flakelib/planar/density/cursor_splatter.hpp>
#include <sge/input/cursor/button_event.hpp>
#include <sge/input/cursor/object.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/onscreen_target.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/math/box/contains_point.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/vector/structure_cast.hpp>
#include <fcppt/tr1/functional.hpp>
#include <fcppt/variant/object.hpp>


namespace
{
sge::input::cursor::position const
window_position_to_viewport_position(
	sge::renderer::device &_renderer,
	sge::input::cursor::position const &_position)
{
	return
		_position -
		fcppt::math::vector::structure_cast<sge::input::cursor::position>(
			_renderer.onscreen_target().viewport().get().pos());
}
}

FCPPT_PP_PUSH_WARNING
FCPPT_PP_DISABLE_VC_WARNING(4355)
flakelib::planar::density::cursor_splatter::cursor_splatter(
	sge::opencl::command_queue::object &_command_queue,
	density::source_image const &_source_image,
	sge::input::cursor::object &_cursor,
	sge::renderer::device &_renderer,
	flakelib::build_options const &_build_options,
	density::splat_radius const &_splat_radius)
:
	command_queue_(
		_command_queue),
	source_image_(
		_source_image.get()),
	cursor_(
		_cursor),
	renderer_(
		_renderer),
	splat_radius_(
		_splat_radius.get()),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/planar/density_splatter.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	splat_kernel_(
		program_,
		sge::opencl::kernel::name(
			"splat")),
	cursor_button_connection_(
		cursor_.button_callback(
			std::tr1::bind(
				&cursor_splatter::cursor_button_callback,
				this,
				std::tr1::placeholders::_1))),
	cursor_move_connection_(
		cursor_.move_callback(
			std::tr1::bind(
				&cursor_splatter::cursor_move_callback,
				this,
				std::tr1::placeholders::_1))),
	is_being_dragged_(
		false)
{
}
FCPPT_PP_POP_WARNING

void
flakelib::planar::density::cursor_splatter::update_cursor_rectangle(
	density::cursor_rectangle const &_cursor_rectangle)
{
	cursor_rectangle_ =
		_cursor_rectangle;
}

flakelib::planar::density::cursor_splatter::~cursor_splatter()
{
}

void
flakelib::planar::density::cursor_splatter::cursor_button_callback(
	sge::input::cursor::button_event const &e)
{
	switch(e.button_code())
	{
		case sge::input::cursor::button_code::right:
			is_being_dragged_ =
				e.pressed();
			break;
		case sge::input::cursor::button_code::left:
			if(e.pressed())
				this->splat_at_cursor_position();
			break;
		case sge::input::cursor::button_code::middle:
			break;
		case sge::input::cursor::button_code::unknown:
			break;
	}
}

void
flakelib::planar::density::cursor_splatter::cursor_move_callback(
	sge::input::cursor::move_event const &)
{
	if(!is_being_dragged_ || !cursor_.position())
		return;

	this->splat_at_cursor_position();
}

void
flakelib::planar::density::cursor_splatter::splat_at_cursor_position()
{
	sge::input::cursor::position const cursor_position(
		::window_position_to_viewport_position(
			renderer_,
			*cursor_.position()));

	if(
		!fcppt::math::box::contains_point(
			cursor_rectangle_,
			cursor_position))
		return;

	sge::input::cursor::position const
		relative_position(
			cursor_position - cursor_rectangle_.pos()),
		grid_position(
			relative_position *
			fcppt::math::dim::structure_cast<sge::input::cursor::position>(
				source_image_.size()) /
			fcppt::math::dim::structure_cast<sge::input::cursor::position>(
				cursor_rectangle_.size()));

	splat_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		source_image_.buffer());

	splat_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		static_cast<cl_int>(
			source_image_.size()[0]));

	splat_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		static_cast<cl_int>(
			grid_position.x()));

	splat_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_int>(
			grid_position.y()));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		splat_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(splat_radius_)
			(splat_radius_).container());
}
