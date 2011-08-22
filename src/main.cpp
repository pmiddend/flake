#include "media_path.hpp"
#include "simulation.hpp"
#include <sge/config/media_path.hpp>
#include <sge/font/metrics_ptr.hpp>
#include <sge/font/rect.hpp>
#include <sge/font/size_type.hpp>
#include <sge/font/system.hpp>
#include <sge/font/text/align_h.hpp>
#include <sge/font/text/align_v.hpp>
#include <sge/font/text/drawer_3d.hpp>
#include <sge/font/text/draw.hpp>
#include <sge/font/text/flags_none.hpp>
#include <sge/font/text/from_fcppt_string.hpp>
#include <sge/font/text/lit.hpp>
#include <sge/font/text/part.hpp>
#include <sge/font/text/string.hpp>
#include <sge/parse/json/config/merge_command_line_parameters.hpp>
#include <sge/parse/json/config/create_command_line_parameters.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/path.hpp>
#include <sge/parse/json/parse_file_exn.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/array.hpp>
#include <sge/image/colors.hpp>
#include <sge/input/cursor/demuxer.hpp>
#include <sge/input/cursor/move_event.hpp>
#include <sge/input/cursor/button_event.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/key_code.hpp>
#include <sge/input/keyboard/key_event.hpp>
#include <sge/renderer/active_target.hpp>
#include <sge/renderer/aspect.hpp>
#include <sge/renderer/depth_stencil_buffer.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/device_ptr.hpp>
#include <sge/renderer/no_multi_sampling.hpp>
#include <sge/renderer/parameters.hpp>
#include <sge/renderer/scoped_block.hpp>
#include <sge/renderer/vector2.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/trampoline.hpp>
#include <sge/renderer/state/var.hpp>
#include <sge/renderer/target_base.hpp>
#include <sge/renderer/visual_depth.hpp>
#include <sge/renderer/vsync.hpp>
#include <sge/systems/cursor_option_field.hpp>
#include <sge/systems/cursor_option.hpp>
#include <sge/systems/input_helper_field.hpp>
#include <sge/systems/input_helper.hpp>
#include <sge/systems/input.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <sge/systems/parameterless.hpp>
#include <sge/systems/renderer.hpp>
#include <sge/systems/running_to_false.hpp>
#include <sge/systems/window.hpp>
#include <sge/timer/frames_counter.hpp>
#include <sge/viewport/center_on_resize.hpp>
#include <sge/viewport/manager.hpp>
#include <sge/window/dim.hpp>
#include <sge/window/instance.hpp>
#include <sge/window/simple_parameters.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/tr1/functional.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/lexical_cast.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/math/vector/structure_cast.hpp>
#include <fcppt/math/vector/construct.hpp>
#include <fcppt/math/dim/arithmetic.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/text.hpp>
#include <exception>
#include <iostream>
#include <ostream>
#include <cstdlib>

int
main(
	int const argc,
	char *argv[])
try
{
	sge::parse::json::object const config_file(
		sge::parse::json::config::merge_command_line_parameters(
			sge::parse::json::parse_file_exn(
				flake::media_path()/FCPPT_TEXT("config.json")),
			sge::parse::json::config::create_command_line_parameters(
				argc,
				argv)));

	sge::systems::instance sys(
		sge::systems::list()
		(sge::systems::window(
			sge::window::simple_parameters(
				FCPPT_TEXT("flake 2D example"),
				sge::parse::json::find_and_convert_member<sge::window::dim>(
					config_file,
					sge::parse::json::path(
						FCPPT_TEXT("window-size"))))))
		(sge::systems::renderer(
				sge::renderer::parameters(
					sge::renderer::visual_depth::depth32,
					sge::renderer::depth_stencil_buffer::off,
					sge::renderer::vsync::on,
					sge::renderer::no_multi_sampling),
				sge::viewport::center_on_resize(
					sge::parse::json::find_and_convert_member<sge::window::dim>(
						config_file,
						sge::parse::json::path(
							FCPPT_TEXT("window-size"))))))
		(sge::systems::input(
				sge::systems::input_helper_field(
					sge::systems::input_helper::mouse_collector)
					| sge::systems::input_helper::cursor_demuxer
					| sge::systems::input_helper::keyboard_collector,
				sge::systems::cursor_option_field::null()))
		(sge::systems::parameterless::font));

	bool running =
		true;

	fcppt::signal::scoped_connection const input_connection(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::escape,
				sge::systems::running_to_false(
					running))));

	sge::font::metrics_ptr const font_metrics(
		sys.font_system().create_font(
			sge::config::media_path() / FCPPT_TEXT("fonts") / FCPPT_TEXT("default.ttf"),
			static_cast<sge::font::size_type>(
				30)));

	sge::font::text::drawer_3d font_drawer(
		sys.renderer(),
		sge::image::colors::red());

	sys.renderer().state(
		sge::renderer::state::list
			(sge::renderer::state::bool_::clear_back_buffer = true)
			(sge::renderer::state::color::back_buffer_clear_color = sge::image::colors::black()));

	sge::timer::frames_counter frames_counter;

	flake::simulation sim(
		config_file,
		sys);

	while(
		running)
	{
		sys.window().dispatch();
		frames_counter.update();
		sim.update();

		sge::renderer::scoped_block const block(
			sys.renderer());

		sim.render();

#if 0
		sge::font::text::draw(
			*font_metrics,
			font_drawer,
			SGE_FONT_TEXT_LIT("Press the left mouse button to set a point"),
			sge::font::rect(
				sge::font::rect::vector::null(),
				fcppt::math::dim::structure_cast<sge::font::rect::dim>(
					sge::renderer::active_target(
						sys.renderer()
					).viewport().get().size())),
			sge::font::text::align_h::left,
			sge::font::text::align_v::top,
			sge::font::text::flags::none);
#endif

		sge::font::text::draw(
			*font_metrics,
			font_drawer,
			sge::font::text::from_fcppt_string(
				frames_counter.frames_str())
			+ SGE_FONT_TEXT_LIT(" fps"),
			sge::font::rect(
				sge::font::rect::vector::null(),
				fcppt::math::dim::structure_cast<sge::font::rect::dim>(
					sge::renderer::active_target(
						sys.renderer()
					).viewport().get().size())),
			sge::font::text::align_h::right,
			sge::font::text::align_v::top,
			sge::font::text::flags::none);
	}
}
catch(
	fcppt::exception const &_error)
{
	fcppt::io::cerr
		<< _error.string()
		<< FCPPT_TEXT('\n');

	return EXIT_FAILURE;
}
 catch(
	std::exception const &_error)
{
	std::cerr
		<< _error.what()
		<< '\n';

	return EXIT_FAILURE;
}
