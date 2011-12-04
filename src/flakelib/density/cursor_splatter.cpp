#include <flakelib/density/cursor_splatter.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/input/cursor/object.hpp>
#include <sge/input/cursor/button_event.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/math/box/contains_point.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/tr1/functional.hpp>

flakelib::density::cursor_splatter::cursor_splatter(
	sge::opencl::command_queue::object &_command_queue,
	sge::opencl::memory_object::image::planar &_source_image,
	sge::input::cursor::object &_cursor,
	density::splat_radius const &_splat_radius)
:
	command_queue_(
		_command_queue),
	source_image_(
		_source_image),
	cursor_(
		_cursor),
	splat_radius_(
		_splat_radius.get()),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/density_splatter.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters())),
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

void
flakelib::density::cursor_splatter::update_cursor_rectangle(
	density::cursor_rectangle const &_cursor_rectangle)
{
	cursor_rectangle_ =
		_cursor_rectangle;
}

flakelib::density::cursor_splatter::~cursor_splatter()
{
}

void
flakelib::density::cursor_splatter::cursor_button_callback(
	sge::input::cursor::button_event const &e)
{
	if(e.button_code() != sge::input::cursor::button_code::left)
		return;

	is_being_dragged_ =
		e.pressed();
}

void
flakelib::density::cursor_splatter::cursor_move_callback(
	sge::input::cursor::move_event const &)
{
	if(!is_being_dragged_ || !cursor_.position())
		return;

	sge::input::cursor::position const cursor_position(
		*cursor_.position());

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
		source_image_);

	splat_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		static_cast<cl_int>(
			grid_position.x()));

	splat_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		static_cast<cl_int>(
			grid_position.y()));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		splat_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(splat_radius_)
			(splat_radius_).container(),
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(1)
			(1).container());
}
