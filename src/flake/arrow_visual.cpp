#include <flakelib/duration.hpp>
#include <flakelib/exception.hpp>
#include <flakelib/media_path.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/planar_framework.hpp>
#include <flakelib/utf8_file_to_fcppt_string.hpp>
#include <flakelib/laplace_solver/dynamic_factory.hpp>
#include <flakelib/planar_pool/object.hpp>
#include <flakelib/simulation/stam/object.hpp>
#include <flakelib/utility/object.hpp>
#include <flakelib/arrow_visual.hpp>
#include <sge/config/media_path.hpp>
#include <sge/console/arg_list.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/console/gfx.hpp>
#include <sge/console/muxing_fcppt_streambuf.hpp>
#include <sge/console/muxing_narrow_streambuf.hpp>
#include <sge/console/object.hpp>
#include <sge/console/sprite_object.hpp>
#include <sge/console/sprite_parameters.hpp>
#include <sge/console/callback/from_functor.hpp>
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
#include <sge/opencl/memory_object/create_image_format.hpp>
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
#include <sge/window/system.hpp>
#include <sge/window/parameters.hpp>
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
#include <fcppt/config/platform.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/tr1/functional.hpp>
#include <fcppt/config/external_begin.hpp>
#include <cstddef>
#include <exception>
#include <iostream>
#include <ostream>
#include <fcppt/config/external_end.hpp>
#include <flakelib/main_head.hpp>

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
						FCPPT_TEXT("Simulation of fluid dynamics via Stam's Method")),
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
					sge::systems::input_helper::keyboard_collector) | sge::systems::input_helper::cursor_demuxer,
				sge::systems::cursor_option_field::null())));

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

	sge::image2d::file_ptr boundary_image =
		sys.image_system().load(
			flakelib::media_path()
				/ FCPPT_TEXT("images")
				/
				sge::parse::json::find_and_convert_member<fcppt::string>(
					config_file,
					sge::parse::json::string_to_path(FCPPT_TEXT("stam-test/boundary-file"))));

	flakelib::planar_pool::object scalar_pool(
		opencl_system.context(),
		sge::opencl::memory_object::create_image_format(
			CL_R,
			CL_FLOAT));

	flakelib::planar_pool::object arrow_pool(
		opencl_system.context(),
		sge::opencl::memory_object::create_image_format(
			CL_RG,
			CL_FLOAT));

	flakelib::utility::object utility_object(
		opencl_system.command_queue(),
		global_build_options);

	flakelib::laplace_solver::dynamic_factory configurable_solver(
		scalar_pool,
		opencl_system.command_queue(),
		sge::parse::json::find_and_convert_member<sge::parse::json::object>(
			config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("simulation-solver"))),
		global_build_options,
		utility_object);

	flakelib::simulation::stam::object simulation(
		opencl_system.command_queue(),
		flakelib::boundary_view(
			boundary_image->view()),
		sge::parse::json::find_and_convert_member<sge::parse::json::object>(
			config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("stam-test"))),
		global_build_options,
		flakelib::simulation::arrow_image_cache(
			arrow_pool),
		flakelib::simulation::scalar_image_cache(
			scalar_pool),
		utility_object,
		configurable_solver.value());

	flakelib::arrow_visual visualization(
		simulation,
		sys.renderer(),
		opencl_system.command_queue(),
		sys.font_system(),
		sge::parse::json::find_and_convert_member<sge::parse::json::object>(
			config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("arrow-visualization"))),
		global_build_options,
		sys.viewport_manager(),
		flakelib::boundary_view(
			boundary_image->view()));

	bool running =
		true;

	fcppt::signal::scoped_connection const cb(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::escape,
				sge::systems::running_to_false(
					running))));

	sge::timer::basic<sge::timer::clocks::standard> delta_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			fcppt::chrono::seconds(1)));

	flakelib::duration delta(0.0f);

	while(running)
	{
		sys.window_system().poll();

		delta +=
			50.0f * sge::timer::elapsed_and_reset<flakelib::duration>(
				delta_timer);

		simulation.update(
			delta);

		visualization.update(
			delta);

		delta = flakelib::duration(0.0f);

		// If we have no viewport (yet), don't do anything (this is just a
		// precaution, we _might_ divide by zero somewhere below, otherwise)
		if(!sge::renderer::viewport_size(sys.renderer()).content())
			throw flakelib::exception(
				FCPPT_TEXT("There was an iteration without viewport. Usually not a problem, but OpenCL isn't suited for viewport changes, yet. So I have to exit now."));

		sge::renderer::state::scoped scoped_state(
			sys.renderer(),
			sge::renderer::state::list
				(sge::renderer::state::bool_::clear_back_buffer = true)
				(sge::renderer::state::color::back_buffer_clear_color = sge::image::colors::paleturquoise()));

		sge::renderer::scoped_block const block_(
			sys.renderer());

		visualization.render();
	}
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

