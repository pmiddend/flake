#include <sge/renderer/state/list.hpp>
#include <sge/window/object.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/exception.hpp>
#include <flakelib/media_path.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/utf8_file_to_fcppt_string.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/planar/arrows_only.hpp>
#include <flakelib/planar/laplace_solver/dynamic_factory.hpp>
#include <flakelib/planar/simulation/stam/object.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/config/media_path.hpp>
#include <sge/console/arg_list.hpp>
#include <sge/console/gfx.hpp>
#include <sge/console/muxing_fcppt_streambuf.hpp>
#include <sge/console/muxing_narrow_streambuf.hpp>
#include <sge/console/object.hpp>
#include <sge/console/sprite_object.hpp>
#include <sge/console/sprite_parameters.hpp>
#include <sge/console/callback/from_functor.hpp>
#include <sge/camera/ortho_freelook/object.hpp>
#include <sge/camera/ortho_freelook/parameters.hpp>
#include <sge/font/system.hpp>
#include <sge/font/text/lit.hpp>
#include <sge/image/capabilities_field.hpp>
#include <sge/image/colors.hpp>
#include <sge/image2d/file.hpp>
#include <sge/image2d/file_ptr.hpp>
#include <sge/image2d/system.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/key_code.hpp>
#include <sge/log/global_context.hpp>
#include <sge/media/all_extensions.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/opencl/single_device_system/parameters.hpp>
#include <sge/parse/json/array.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/parse_string_exn.hpp>
#include <sge/parse/json/path.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/parse/json/config/create_command_line_parameters.hpp>
#include <sge/parse/json/config/merge_command_line_parameters.hpp>
#include <sge/renderer/depth_stencil_buffer.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/no_multi_sampling.hpp>
#include <sge/renderer/parameters.hpp>
#include <sge/renderer/scoped_block.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/renderer/viewport_size.hpp>
#include <sge/renderer/visual_depth.hpp>
#include <sge/renderer/vsync.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/sprite/parameters.hpp>
#include <sge/systems/cursor_option_field.hpp>
#include <sge/systems/image2d.hpp>
#include <sge/systems/input.hpp>
#include <sge/systems/input_helper.hpp>
#include <sge/systems/input_helper_field.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <sge/systems/renderer.hpp>
#include <sge/systems/running_to_false.hpp>
#include <sge/systems/window.hpp>
#include <sge/timer/basic.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/parameters.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <sge/viewport/center_on_resize.hpp>
#include <sge/window/dim.hpp>
#include <sge/window/parameters.hpp>
#include <sge/window/system.hpp>
#include <fcppt/cref.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/to_std_string.hpp>
#include <fcppt/chrono/duration_comparison.hpp>
#include <fcppt/chrono/seconds.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/io/clog.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/log/activate_levels.hpp>
#include <fcppt/log/context.hpp>
#include <fcppt/log/level.hpp>
#include <fcppt/log/location.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/tr1/functional.hpp>
#include <fcppt/config/external_begin.hpp>
#include <cstddef>
#include <exception>
#include <iostream>
#include <ostream>
#include <fcppt/config/external_end.hpp>
// Separator
#include <flakelib/main_head.hpp>

// DEBUG
#include <fcppt/math/dim/output.hpp>


namespace
{
void
update_simulation_and_visualization(
	flakelib::planar::simulation::stam::object &_simulation,
	flakelib::planar::arrows_only &_visual,
	flakelib::duration const &_delta)
{
			_simulation.update(
				_delta);

			_visual.update(
				_delta);
}

void
toggle_console_active(
	sge::console::gfx &_console_gfx)
{
	_console_gfx.active(
		!_console_gfx.active());
}

void
stats_console_callback(
	flakelib::planar::simulation::stam::object const &_simulation)
{
	fcppt::io::cout() << _simulation.parent_profiler() << FCPPT_TEXT("\n");
}
}

FLAKELIB_MAIN_HEAD
try
{
	sge::log::global_context().apply(
		fcppt::log::location(
			FCPPT_TEXT("opencl")),
		std::tr1::bind(
			&fcppt::log::activate_levels,
			std::tr1::placeholders::_1,
			fcppt::log::level::verbose));

	sge::parse::json::object const config_file =
		sge::parse::json::parse_string_exn(
			flakelib::utf8_file_to_fcppt_string(
				flakelib::media_path_from_string(
					FCPPT_TEXT("config.json"))));

	sge::window::dim const window_size =
		sge::parse::json::find_and_convert_member<sge::window::dim>(
			config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("window-size")));

	sge::systems::instance sys(
		sge::systems::list()
			(sge::systems::image2d(
				sge::image::capabilities_field::null(),
				sge::media::all_extensions))
			(sge::systems::window(
				sge::window::parameters(
					sge::window::title(
						FCPPT_TEXT("Step by step 2D simulation - arrows only")),
					window_size)))
			(sge::systems::renderer(
				sge::renderer::parameters(
					sge::renderer::visual_depth::depth32,
					sge::renderer::depth_stencil_buffer::off,
					sge::renderer::vsync::on,
					sge::renderer::no_multi_sampling),
				sge::viewport::center_on_resize(
					window_size)))
			(sge::systems::font())
			(sge::systems::input(
				sge::systems::input_helper_field(
					sge::systems::input_helper::keyboard_collector)
					| sge::systems::input_helper::cursor_demuxer
					| sge::systems::input_helper::mouse_collector,
				sge::systems::cursor_option_field(
					sge::systems::cursor_option::exclusive))));

	sge::font::metrics_ptr const font_metrics(
		sys.font_system().create_font(
			sge::config::media_path()
				/ FCPPT_TEXT("fonts")
				/ FCPPT_TEXT("default.ttf"),
			15));

	sge::console::object console_object(
		SGE_FONT_TEXT_LIT('/'));

	sge::console::muxing_fcppt_streambuf fcpptout_streambuf(
		fcppt::io::cout(),
		console_object,
		sge::console::muxing::enabled);

	sge::console::muxing_fcppt_streambuf fcppterr_streambuf(
		fcppt::io::cerr(),
		console_object,
		sge::console::muxing::enabled);

	sge::console::muxing_fcppt_streambuf fcpptlog_streambuf(
		fcppt::io::clog(),
		console_object,
		sge::console::muxing::enabled);

	sge::console::gfx console_gfx(
		console_object,
		sys.renderer(),
		sge::image::colors::black(),
		*font_metrics,
		sys.keyboard_collector(),
		sge::console::sprite_object(
			sge::console::sprite_parameters()
			.pos(
				sge::console::sprite_object::vector::null())
			.size(
				sge::console::sprite_object::dim(
					static_cast<sge::console::sprite_object::unit>(
						window_size.w()),
					static_cast<sge::console::sprite_object::unit>(
						window_size.h()/2)))),
		static_cast<sge::console::output_line_limit>(
			100));

	fcppt::signal::scoped_connection const console_cb(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::f1,
				std::tr1::bind(
					&toggle_console_active,
					fcppt::ref(
						console_gfx)))));

	sge::camera::ortho_freelook::object camera(
		sge::camera::ortho_freelook::parameters(
			sys.mouse_collector(),
			sys.keyboard_collector(),
			sge::renderer::projection::rect(
				sge::renderer::projection::rect::vector::null(),
				fcppt::math::dim::structure_cast<sge::renderer::projection::rect::dim>(
					window_size)))
			.zoom_to_zooming_speed_factor(
				sge::renderer::vector2(
					static_cast<sge::renderer::scalar>(
						0.01),
					static_cast<sge::renderer::scalar>(
						0.01))));

	sge::opencl::single_device_system::object opencl_system(
		sge::opencl::single_device_system::parameters()
			.renderer(
				sys.renderer())
			.prefer_gpu());

	flakelib::build_options global_build_options(
		std::string(
			"-cl-mad-enable -cl-fast-relaxed-math -Werror ")+
		"-I "+
		fcppt::to_std_string(
			fcppt::filesystem::path_to_string(
				flakelib::media_path_from_string(
					FCPPT_TEXT("kernels")))));

	sge::parse::json::path const json_prefix(
		FCPPT_TEXT("arrows-only-test"));

	sge::image2d::file_ptr boundary_image =
		sys.image_system().load(
			flakelib::media_path()
				/ FCPPT_TEXT("boundaries")
				/
				sge::parse::json::find_and_convert_member<fcppt::string>(
					config_file,
					json_prefix / FCPPT_TEXT("boundary-file")));

	flakelib::buffer_pool::object scalar_pool(
		opencl_system.context());

	flakelib::utility::object utility_object(
		opencl_system.command_queue(),
		global_build_options);

	flakelib::planar::laplace_solver::dynamic_factory configurable_solver(
		scalar_pool,
		opencl_system.command_queue(),
		sge::parse::json::find_and_convert_member<sge::parse::json::object>(
			config_file,
			json_prefix / FCPPT_TEXT("simulation") / FCPPT_TEXT("solver")),
		global_build_options,
		utility_object);

	flakelib::planar::simulation::stam::object simulation(
		opencl_system.command_queue(),
		flakelib::planar::boundary_view(
			boundary_image->view()),
		global_build_options,
		scalar_pool,
		utility_object,
		configurable_solver.value(),
		flakelib::planar::simulation::stam::external_force_magnitude(
			sge::parse::json::find_and_convert_member<cl_float>(
				config_file,
				json_prefix / FCPPT_TEXT("simulation") / FCPPT_TEXT("external-force-magnitude"))),
		flakelib::planar::simulation::stam::gravity_magnitude(
			sge::parse::json::find_and_convert_member<cl_float>(
				config_file,
				json_prefix / FCPPT_TEXT("simulation") / FCPPT_TEXT("gravity-magnitude"))),
		flakelib::planar::simulation::stam::profiling_enabled(
			sge::parse::json::find_and_convert_member<bool>(
				config_file,
				json_prefix / FCPPT_TEXT("simulation") / FCPPT_TEXT("profiling-enabled"))));

	fcppt::signal::scoped_connection const stats_cb(
		console_object.insert(
			sge::console::callback::from_functor<void()>(
				std::tr1::bind(
					&stats_console_callback,
					fcppt::cref(
						simulation)),
				sge::console::callback::name(
					SGE_FONT_TEXT_LIT("stats")),
				sge::console::callback::short_description(
					SGE_FONT_TEXT_LIT("Usage: /stats")))
				.long_description(
					SGE_FONT_TEXT_LIT("Outputs simulation profiling statistics"))));

	flakelib::planar::arrows_only visualization(
		sys.viewport_manager(),
		sys.renderer(),
		opencl_system.command_queue(),
		simulation,
		sys.font_system(),
		global_build_options,
		flakelib::planar::boundary_view(
			boundary_image->view()),
		sge::parse::json::find_and_convert_member<sge::parse::json::object>(
			config_file,
			json_prefix));

	bool running =
		true;

	fcppt::signal::scoped_connection const cb(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::escape,
				sge::systems::running_to_false(
					running))));

	fcppt::signal::scoped_connection const cb_sim(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::space,
				std::tr1::bind(
					&update_simulation_and_visualization,
					fcppt::ref(
						simulation),
					fcppt::ref(
						visualization),
					flakelib::duration(
						1.00f)))));

	sge::timer::basic<sge::timer::clocks::standard> delta_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			fcppt::chrono::seconds(1)));

	flakelib::duration delta(0.0f);

	sge::timer::basic<sge::timer::clocks::standard> camera_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			sge::camera::duration(
				1.0f)));

	sys.window_system().poll();

	simulation.update(
		flakelib::duration(0.0f));

	visualization.update(
		flakelib::duration(0.0f));

	while(running)
	{
		sys.window_system().poll();

		camera.update(
			sge::timer::elapsed<sge::camera::duration>(
				camera_timer));

		/*
		delta +=
			10.0f * sge::timer::elapsed_and_reset<flakelib::duration>(
				delta_timer);

		{
			simulation.update(
				delta);

			visualization.update(
				delta);

			delta = flakelib::duration(0.0f);
		}
		*/

		// If we have no viewport (yet), don't do anything (this is just a
		// precaution, we _might_ divide by zero somewhere below, otherwise)
		if(!sge::renderer::viewport_size(sys.renderer()).content())
			throw flakelib::exception(
				FCPPT_TEXT("There was an iteration without viewport. Usually not a problem, but OpenCL isn't suited for viewport changes, yet. So I have to exit now."));

		sge::renderer::state::scoped scoped_state(
			sys.renderer(),
			visualization.render_states());

		sge::renderer::scoped_block const block_(
			sys.renderer());

		visualization.render(
			camera.projection());

		if(console_gfx.active())
			console_gfx.render();
	}

	return EXIT_SUCCESS;
}
catch(fcppt::exception const &e)
{
	fcppt::io::cerr() << FCPPT_TEXT("fcppt exception caught: ") << e.string() << FCPPT_TEXT("\n");
	return EXIT_FAILURE;
}
catch(std::exception const &e)
{
	std::cerr << "standard exception caught: " << e.what() << "\n";
	return EXIT_FAILURE;
}
catch(...)
{
	std::cerr << "unknown exception caught\n";
	return EXIT_FAILURE;
}
