#include <flakelib/duration.hpp>
#include <flakelib/media_path.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/utf8_file_to_fcppt_string.hpp>
#include <flakelib/simulation/base.hpp>
#include <flakelib/simulation/base_ptr.hpp>
#include <flakelib/simulation/create.hpp>
#include <flakelib/visualization/arrow.hpp>
#include <sge/all_extensions.hpp>
#include <sge/image/capabilities_field.hpp>
#include <sge/image2d/file.hpp>
#include <sge/image2d/file_ptr.hpp>
#include <sge/image2d/multi_loader.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/key_code.hpp>
#include <sge/log/global_context.hpp>
#include <sge/opencl/single_device_system.hpp>
#include <sge/parse/json/array.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/parse_string_exn.hpp>
#include <sge/parse/json/path.hpp>
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
#include <sge/timer/basic.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/parameters.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <sge/viewport/center_on_resize.hpp>
#include <sge/window/dim.hpp>
#include <sge/window/instance.hpp>
#include <sge/window/simple_parameters.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/text.hpp>
#include <fcppt/chrono/duration_comparison.hpp>
#include <fcppt/chrono/seconds.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/log/activate_levels.hpp>
#include <fcppt/log/context.hpp>
#include <fcppt/log/level.hpp>
#include <fcppt/log/location.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/config/external_begin.hpp>
#include <cstddef>
#include <exception>
#include <iostream>
#include <ostream>
#include <fcppt/config/external_end.hpp>


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
				flakelib::utf8_file_to_fcppt_string(
					flakelib::media_path_from_string(
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
			(sge::systems::parameterless::font)
			(sge::systems::input(
				sge::systems::input_helper_field(
					sge::systems::input_helper::keyboard_collector),
				sge::systems::cursor_option_field::null())));

	sge::opencl::single_device_system opencl_system(
		(sys.renderer()),
		(sge::opencl::context::optional_error_callback()));

	sge::image2d::file_ptr boundary_image =
		sys.image_loader().load(
			flakelib::media_path()
				/ FCPPT_TEXT("images")
				/ sge::parse::json::find_and_convert_member<fcppt::string>(
					config_file,
					sge::parse::json::path(FCPPT_TEXT("boundary-file"))));

	flakelib::simulation::base_ptr simulation(
		flakelib::simulation::create(
			opencl_system.context(),
			opencl_system.command_queue(),
			flakelib::boundary_view(
				boundary_image->view()),
			config_file));

	flakelib::visualization::arrow visualization(
		sys.viewport_manager(),
		sys.renderer(),
		opencl_system.command_queue(),
		*simulation,
		sys.font_system(),
		flakelib::boundary_view(
			boundary_image->view()),
		config_file);

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
					&flakelib::simulation::base::update,
					simulation.get(),
					flakelib::duration(
						0.01f)))));

	sge::timer::basic<sge::timer::clocks::standard> delta_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			fcppt::chrono::seconds(1)));

	flakelib::duration delta(0.0f);

	while(running)
	{
		sys.window().dispatch();

		delta +=
			sge::timer::elapsed_and_reset<flakelib::duration>(
				delta_timer);

		if(delta > flakelib::duration(0.05f))
		{
			simulation->update(
				delta);

			visualization.update(
				delta);

			delta = flakelib::duration(0.0f);
		}

		// If we have no viewport (yet), don't do anything (this is just a
		// precaution, we _might_ divide by zero somewhere below, otherwise)
		if(!sge::renderer::viewport_size(sys.renderer()).content())
			throw sge::exception(
				FCPPT_TEXT("There was an iteration without viewport. Usually not a problem, but OpenCL isn't suited for viewport changes, yet. So I have to exit now."));

		sge::renderer::state::scoped scoped_state(
			sys.renderer(),
			visualization.render_states());

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

