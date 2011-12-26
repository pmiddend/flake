#include <flakelib/exception.hpp>
#include <flakelib/laplace_tester.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/scoped_frame_limiter.hpp>
#include <flakelib/laplace_solver/jacobi.hpp>
#include <flakelib/laplace_solver/multigrid.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/exception.hpp>
#include <sge/camera/ortho_freelook/object.hpp>
#include <sge/camera/ortho_freelook/parameters.hpp>
#include <sge/image/capabilities_field.hpp>
#include <sge/image/colors.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/key_code.hpp>
#include <sge/log/global_context.hpp>
#include <sge/media/all_extensions.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/opencl/single_device_system/parameters.hpp>
#include <sge/renderer/depth_stencil_buffer.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/no_multi_sampling.hpp>
#include <sge/renderer/parameters.hpp>
#include <sge/renderer/scoped_block.hpp>
#include <sge/renderer/viewport_size.hpp>
#include <sge/renderer/visual_depth.hpp>
#include <sge/renderer/vsync.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/state/trampoline.hpp>
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
#include <sge/timer/elapsed.hpp>
#include <sge/timer/elapsed_fractional.hpp>
#include <sge/timer/parameters.hpp>
#include <sge/timer/reset_when_expired.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <sge/viewport/center_on_resize.hpp>
#include <sge/window/dim.hpp>
#include <sge/window/system.hpp>
#include <sge/window/parameters.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/text.hpp>
#include <fcppt/to_std_string.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/config/external_begin.hpp>
#include <exception>
#include <iostream>
#include <ostream>
#include <fcppt/config/external_end.hpp>
#include <flakelib/main_head.hpp>


FLAKELIB_MAIN_HEAD
try
{
	sge::window::dim const window_dim(
		1920,
		1080);

	sge::systems::instance sys(
		sge::systems::list()
			(sge::systems::image2d(
				sge::image::capabilities_field::null(),
				sge::media::all_extensions))
			(sge::systems::window(
				sge::window::parameters(
					sge::window::title(
						FCPPT_TEXT("Test of different pde solvers")),
					sge::window::dim(1920,1080))))
			(sge::systems::renderer(
				sge::renderer::parameters(
					sge::renderer::visual_depth::depth32,
					sge::renderer::depth_stencil_buffer::off,
					sge::renderer::vsync::on,
					sge::renderer::no_multi_sampling),
				sge::viewport::center_on_resize(
					window_dim)))
			(sge::systems::font())
			(sge::systems::input(
				sge::systems::input_helper_field(
					sge::systems::input_helper::keyboard_collector) | sge::systems::input_helper::mouse_collector,
				sge::systems::cursor_option_field(
					sge::systems::cursor_option::exclusive))));

	sge::opencl::single_device_system::object opencl_system(
		sge::opencl::single_device_system::parameters()
			.renderer(
				sys.renderer()));

	flakelib::buffer_pool::object cache(
		opencl_system.context());

	flakelib::build_options global_build_options(
		"-I "+
		fcppt::to_std_string(
			fcppt::filesystem::path_to_string(
				flakelib::media_path_from_string(
					FCPPT_TEXT("kernels")))));

	flakelib::laplace_solver::jacobi jacobi_solver(
		cache,
		opencl_system.command_queue(),
		global_build_options,
		flakelib::laplace_solver::grid_scale(
			1.0f),
		flakelib::laplace_solver::iterations(
			15));

	flakelib::utility::object utility_object(
		opencl_system.command_queue(),
		global_build_options);

	flakelib::laplace_solver::multigrid multigrid_solver(
		cache,
		utility_object,
		opencl_system.command_queue(),
		global_build_options,
		jacobi_solver,
		flakelib::laplace_solver::grid_scale(
			1.0f),
		flakelib::laplace_solver::termination_size(
			64),
		flakelib::laplace_solver::debug_output(
			true));

	flakelib::laplace_tester tester(
		multigrid_solver,
		cache,
		utility_object,
		sys.renderer(),
		sys.viewport_manager(),
		opencl_system.command_queue(),
		sys.font_system(),
		sys.image_system(),
		global_build_options);

	bool running =
		true;

	fcppt::signal::scoped_connection const cb(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::escape,
				sge::systems::running_to_false(
					running))));

	tester.update();

	sys.window_system().poll();

	sge::camera::ortho_freelook::object camera(
		sge::camera::ortho_freelook::parameters(
			sys.mouse_collector(),
			sys.keyboard_collector(),
			sge::renderer::projection::rect(
				sge::renderer::projection::rect::vector::null(),
				sge::renderer::projection::rect::dim(
					static_cast<sge::renderer::scalar>(
						window_dim[0]),
					static_cast<sge::renderer::scalar>(
						window_dim[1]))))
			.zoom_to_zooming_speed_factor(
				sge::renderer::vector2(
					static_cast<sge::renderer::scalar>(
						0.1),
					static_cast<sge::renderer::scalar>(
						0.1))));

	sge::timer::basic<sge::timer::clocks::standard> camera_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			sge::camera::duration(
				1.0f)));

	while(running)
	{
		flakelib::scoped_frame_limiter frame_limiter(
			static_cast<flakelib::scoped_frame_limiter::fps_type>(
				60));

		sys.window_system().poll();

		camera.update(
			sge::timer::elapsed<sge::camera::duration>(
				camera_timer));

		// If we have no viewport (yet), don't do anything (this is just a
		// precaution, we _might_ divide by zero somewhere below, otherwise)
		if(!sge::renderer::viewport_size(sys.renderer()).content())
			throw flakelib::exception(
				FCPPT_TEXT("There was an iteration without viewport.")
				FCPPT_TEXT("Usually not a problem, but OpenCL isn't suited ")
				FCPPT_TEXT("for viewport changes, yet. So I have to exit now."));

		sge::renderer::state::scoped scoped_state(
			sys.renderer(),
			sge::renderer::state::list
				(sge::renderer::state::bool_::clear_back_buffer = true)
				(sge::renderer::state::color::back_buffer_clear_color = sge::image::colors::paleturquoise()));

		sge::renderer::scoped_block const block_(
			sys.renderer());

		tester.render(
			flakelib::monitor::optional_projection(
				camera.projection()));
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

