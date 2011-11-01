#include <flakelib/laplace_tester.hpp>
#include <flakelib/laplace_solver/jacobi.hpp>
#include <flakelib/laplace_solver/multigrid.hpp>
#include <sge/all_extensions.hpp>
#include <sge/exception.hpp>
#include <sge/image/capabilities_field.hpp>
#include <sge/image/colors.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/key_code.hpp>
#include <sge/log/global_context.hpp>
#include <sge/opencl/single_device_system.hpp>
#include <sge/opencl/memory_object/create_image_format.hpp>
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
#include <sge/systems/image_loader.hpp>
#include <sge/systems/input.hpp>
#include <sge/systems/input_helper.hpp>
#include <sge/systems/input_helper_field.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <sge/systems/renderer.hpp>
#include <sge/systems/running_to_false.hpp>
#include <sge/systems/window.hpp>
#include <sge/viewport/center_on_resize.hpp>
#include <sge/window/dim.hpp>
#include <sge/window/instance.hpp>
#include <sge/window/simple_parameters.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/text.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/config/external_begin.hpp>
#include <exception>
#include <iostream>
#include <ostream>
#include <fcppt/config/external_end.hpp>


int
main(
	int,
	char *[])
try
{
	sge::systems::instance sys(
		sge::systems::list()
			(sge::systems::image_loader(
				sge::image::capabilities_field::null(),
				sge::all_extensions))
			(sge::systems::window(
				sge::window::simple_parameters(
					FCPPT_TEXT("Test of different pde solvers"),
					sge::window::dim(1920,1080))))
			(sge::systems::renderer(
				sge::renderer::parameters(
					sge::renderer::visual_depth::depth32,
					sge::renderer::depth_stencil_buffer::off,
					sge::renderer::vsync::on,
					sge::renderer::no_multi_sampling),
				sge::viewport::center_on_resize(
					sge::window::dim(1920,1080))))
			(sge::systems::parameterless::font)
			(sge::systems::input(
				sge::systems::input_helper_field(
					sge::systems::input_helper::keyboard_collector),
				sge::systems::cursor_option_field::null())));

	sge::opencl::single_device_system opencl_system(
		(sys.renderer()),
		(sge::opencl::context::optional_error_callback()));

	flakelib::planar_cache cache(
		opencl_system.context(),
		sge::opencl::memory_object::create_image_format(
			CL_R,
			CL_FLOAT));

	flakelib::laplace_solver::jacobi jacobi_solver(
		cache,
		opencl_system.command_queue(),
		flakelib::laplace_solver::grid_scale(
			1.0f),
		flakelib::laplace_solver::iterations(
			3));

	flakelib::laplace_solver::multigrid multigrid_solver(
		cache,
		opencl_system.command_queue(),
		jacobi_solver,
		flakelib::laplace_solver::grid_scale(
			1.0f));

	flakelib::laplace_tester tester(
		multigrid_solver,
		cache,
		sys.renderer(),
		sys.viewport_manager(),
		opencl_system.command_queue(),
		sys.font_system(),
		sys.image_loader());

	tester.update();

	bool running =
		true;

	fcppt::signal::scoped_connection const cb(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::escape,
				sge::systems::running_to_false(
					running))));

	sys.window().dispatch();

	while(running)
	{
		sys.window().dispatch();

		// If we have no viewport (yet), don't do anything (this is just a
		// precaution, we _might_ divide by zero somewhere below, otherwise)
		if(!sge::renderer::viewport_size(sys.renderer()).content())
			throw sge::exception(
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

		tester.render();
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
