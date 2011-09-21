#include "simulation/base_ptr.hpp"
#include "media_path_from_string.hpp"
#include "duration.hpp"
#include "utf8_file_to_fcppt_string.hpp"
#include "visualization/base_ptr.hpp"
#include "visualization/base.hpp"
#include "visualization/create.hpp"
#include "simulation/create.hpp"
#include "simulation/base.hpp"
#include <sge/opencl/single_device_system.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/image_loader.hpp>
#include <sge/systems/window.hpp>
#include <sge/systems/list.hpp>
#include <sge/systems/renderer.hpp>
#include <sge/window/simple_parameters.hpp>
#include <sge/window/dim.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/array.hpp>
#include <sge/parse/json/config/merge_command_line_parameters.hpp>
#include <sge/parse/json/parse_string_exn.hpp>
#include <sge/parse/json/config/create_command_line_parameters.hpp>
#include <sge/window/instance.hpp>
#include <sge/image/capabilities_field.hpp>
#include <sge/renderer/parameters.hpp>
#include <sge/renderer/viewport_size.hpp>
#include <sge/renderer/visual_depth.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/renderer/scoped_block.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/depth_stencil_buffer.hpp>
#include <sge/renderer/vsync.hpp>
#include <sge/renderer/no_multi_sampling.hpp>
#include <sge/image2d/file.hpp>
#include <sge/image2d/multi_loader.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/viewport/center_on_resize.hpp>
#include <sge/systems/input.hpp>
#include <sge/systems/input_helper_field.hpp>
#include <sge/systems/input_helper.hpp>
#include <sge/systems/cursor_option_field.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/input/keyboard/key_code.hpp>
#include <sge/timer/parameters.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <sge/timer/basic.hpp>
#include <sge/log/global_context.hpp>
#include <sge/systems/running_to_false.hpp>
#include <sge/all_extensions.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/chrono/seconds.hpp>
#include <fcppt/text.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/log/activate_levels.hpp>
#include <fcppt/log/context.hpp>
#include <fcppt/log/level.hpp>
#include <fcppt/log/location.hpp>
#include <iostream>
#include <exception>
#include <ostream>
#include <cstddef>

int
main(
	int argc,
	char *argv[])
try
{
	// - Create textures (w*h):
	//
	// 	1. Vector field 'v' (rgba, float), vector field 'v2' (to write to)
	//      2. Scalar field 'p' (two channels, float)
	// 	3. Boundary (something small, gray8 maybe)
	//
	// - Create one vertex buffer, size w*h*2, shared with OpenCL.
	//
	// - Set the vector field to all zeroes (load it that way?)
	// - Extract the boundary from a thresholded png image
	//
	// - Execute NS algorithm, resulting in a vector field
	// - Convert vector field to a vertex buffer of arrows
	// - Draw the vertex buffer on the screen

	sge::log::global_context().apply(
		fcppt::log::location(
			FCPPT_TEXT("opencl")),
		std::tr1::bind(
			&fcppt::log::activate_levels,
			std::tr1::placeholders::_1,
			fcppt::log::level::verbose));

	sge::parse::json::object const config_file =
		sge::parse::json::config::merge_command_line_parameters(
			sge::parse::json::parse_string_exn(
				flake::utf8_file_to_fcppt_string(
					flake::media_path_from_string(
						FCPPT_TEXT("config.json")))),
			sge::parse::json::config::create_command_line_parameters(
				argc,
				argv));

	sge::systems::instance sys(
		sge::systems::list()
			(sge::systems::image_loader(
				sge::image::capabilities_field::null(),
				sge::all_extensions))
			(sge::systems::window(
				sge::window::simple_parameters(
					FCPPT_TEXT("Simulation of fluid dynamics via Stam's Method"),
					sge::window::dim(1024,768))))
			(sge::systems::renderer(
				sge::renderer::parameters(
					sge::renderer::visual_depth::depth32,
					sge::renderer::depth_stencil_buffer::off,
					sge::renderer::vsync::on,
					sge::renderer::no_multi_sampling),
				sge::viewport::center_on_resize(
					sge::window::dim(1024,768))))
			(sge::systems::input(
				sge::systems::input_helper_field(
					sge::systems::input_helper::keyboard_collector),
				sge::systems::cursor_option_field::null())));

	sge::opencl::single_device_system opencl_system(
		sys.renderer());

	flake::simulation::base_ptr simulation(
		flake::simulation::create(
			opencl_system.context(),
			opencl_system.queue(),
			flake::boundary_view(
				sys.image_loader().load(
					flake::media_path_from_string(
						FCPPT_TEXT("images/boundary.png")))->view()),
			config_file));

	flake::visualization::base_ptr visualization(
		flake::visualization::create(
			opencl_system.context(),
			opencl_system.queue(),
			*simulation,
			sys.renderer(),
			config_file));

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
					&flake::simulation::base::update,
					simulation.get(),
					flake::duration(
						0.01f)))));

	sge::timer::basic<sge::timer::clocks::standard> delta_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			fcppt::chrono::seconds(1)));

	fcppt::io::cerr << "Beginning\n";

	while(running)
	{
	//	fcppt::io::cerr << "Iteration\n";

		sys.window().dispatch();

		flake::duration const current_delta =
			sge::timer::elapsed_and_reset<flake::duration>(
				delta_timer);

		simulation->update(
			current_delta);

//		fcppt::io::cerr << "Updated simulation\n";

		visualization->update(
			current_delta);

//		fcppt::io::cerr << "Updated visualization\n";

		// If we have no viewport (yet), don't do anything (this is just a
		// precaution, we _might_ divide by zero somewhere below, otherwise)
		if(!sge::renderer::viewport_size(sys.renderer()).content())
			throw sge::exception(FCPPT_TEXT("There was an iteration without viewport. Usually not a problem, but OpenCL isn't suited for viewport changes, yet. So I have to exit now."));

		sge::renderer::state::scoped scoped_state(
			sys.renderer(),
			visualization->render_states());

		sge::renderer::scoped_block const block_(
			sys.renderer());

		visualization->render();
	}
}
catch(fcppt::exception const &e)
{
	fcppt::io::cerr << FCPPT_TEXT("fcppt exception caught: ") << e.string() << FCPPT_TEXT("\n");
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

