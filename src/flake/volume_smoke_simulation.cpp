#include <sge/parse/json/find_member_exn.hpp>
#include <sge/parse/json/find_member.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/console/arg_list.hpp>
#include <sge/console/gfx.hpp>
#include <sge/console/muxing_fcppt_streambuf.hpp>
#include <sge/console/muxing_narrow_streambuf.hpp>
#include <sge/console/object.hpp>
#include <sge/console/sprite_object.hpp>
#include <sge/console/sprite_parameters.hpp>
#include <sge/console/callback/from_functor.hpp>
#include <sge/systems/image2d.hpp>
#include <sge/systems/font.hpp>
#include <sge/sprite/parameters.hpp>
#include <sge/sprite/object_impl.hpp>
#include <sge/font/system.hpp>
#include <sge/font/text/lit.hpp>
#include <sge/media/extension.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/chrono/seconds.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/log/activate_levels.hpp>
#include <fcppt/log/level.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/matrix/basic_impl.hpp>
#include <fcppt/math/twopi.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/text.hpp>
#include <fcppt/to_std_string.hpp>
#include <fcppt/tr1/functional.hpp>
#include <flakelib/build_options.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/utf8_file_to_fcppt_string.hpp>
#include <flakelib/volume/smoke_simulation.hpp>
#include <sge/camera/first_person/object.hpp>
#include <sge/camera/first_person/parameters.hpp>
#include <sge/camera/projection/object.hpp>
#include <sge/camera/projection/update_perspective_from_viewport.hpp>
#include <sge/config/media_path.hpp>
#include <sge/image/capabilities_field.hpp>
#include <sge/image/colors.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/input/keyboard/collector.hpp>
#include <sge/input/keyboard/key_code.hpp>
#include <sge/log/global.hpp>
#include <sge/media/extension.hpp>
#include <sge/media/extension_set.hpp>
#include <sge/media/optional_extension_set.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/size_type.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/opencl/single_device_system/parameters.hpp>
#include <sge/parse/json/config/create_command_line_parameters.hpp>
#include <sge/parse/json/config/merge_command_line_parameters.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/parse_string_exn.hpp>
#include <sge/parse/json/path.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/active_target.hpp>
#include <sge/renderer/depth_stencil_buffer.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/first_vertex.hpp>
#include <sge/renderer/lock_mode.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/renderer/no_multi_sampling.hpp>
#include <sge/renderer/nonindexed_primitive_type.hpp>
#include <sge/renderer/parameters.hpp>
#include <sge/renderer/projection/far.hpp>
#include <sge/renderer/projection/fov.hpp>
#include <sge/renderer/projection/near.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/scoped_block.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/draw_mode.hpp>
#include <sge/renderer/state/float.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/stencil_func.hpp>
#include <sge/renderer/target_base.hpp>
#include <sge/renderer/viewport.hpp>
#include <sge/renderer/visual_depth.hpp>
#include <sge/renderer/vsync.hpp>
#include <sge/systems/cursor_option_field.hpp>
#include <sge/systems/image2d.hpp>
#include <sge/systems/input_helper_field.hpp>
#include <sge/systems/input_helper.hpp>
#include <sge/systems/input.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <sge/systems/renderer.hpp>
#include <sge/systems/running_to_false.hpp>
#include <sge/systems/window.hpp>
#include <sge/timer/basic.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/elapsed_fractional.hpp>
#include <sge/timer/elapsed.hpp>
#include <sge/timer/parameters.hpp>
#include <sge/timer/reset_when_expired.hpp>
#include <sge/viewport/fill_on_resize.hpp>
#include <sge/viewport/manager.hpp>
#include <sge/window/dim.hpp>
#include <sge/window/parameters.hpp>
#include <sge/window/system.hpp>
#include <sge/window/title.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <ostream>
#include <fcppt/config/external_end.hpp>
// Separator
#include <flakelib/main_head.hpp>

namespace
{
void
toggle_console_active(
	sge::console::gfx &_console_gfx,
	sge::camera::base &_camera)
{
	_console_gfx.active(
		!_console_gfx.active());
	_camera.active(
		!_console_gfx.active());
}
}

FLAKELIB_MAIN_HEAD
try
{
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

	sge::systems::instance const sys(
		sge::systems::list()
			(sge::systems::window(
				sge::window::parameters(
					sge::window::title(
						FCPPT_TEXT("Test for 3D stuff")),
					window_size)))
			(sge::systems::renderer(
				sge::renderer::parameters(
					sge::renderer::visual_depth::depth32,
					sge::renderer::depth_stencil_buffer::d24,
					sge::renderer::vsync::on,
					sge::renderer::no_multi_sampling),
				sge::viewport::fill_on_resize()))
			(sge::systems::font())
			(sge::systems::image2d(
				sge::image::capabilities_field::null(),
				sge::media::optional_extension_set(
					fcppt::assign::make_container<sge::media::extension_set>(
						sge::media::extension(
							FCPPT_TEXT("png"))))))
			(sge::systems::input(
				sge::systems::input_helper_field(
					sge::systems::input_helper::keyboard_collector) | sge::systems::input_helper::mouse_collector,
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

	flakelib::build_options global_build_options(
		std::string(
			"-cl-mad-enable -cl-fast-relaxed-math -Werror ")+
		"-I "+
		fcppt::to_std_string(
			fcppt::filesystem::path_to_string(
				flakelib::media_path_from_string(
					FCPPT_TEXT("kernels")))));

	sge::opencl::single_device_system::object opencl_system(
		sge::opencl::single_device_system::parameters()
			.renderer(
				sys.renderer()));

	bool running =
		true;

	fcppt::signal::scoped_connection const cb(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::escape,
				sge::systems::running_to_false(
					running))));

	sge::camera::first_person::object camera(
		sge::camera::first_person::parameters(
			sge::camera::first_person::movement_speed(
				12.f),
			sge::camera::first_person::rotation_speed(
				200.f),
			sys.keyboard_collector(),
			sys.mouse_collector())
			.gizmo(
				sge::camera::gizmo_type()
					.position(
						sge::renderer::vector3(20.5166f,76.4709f,35.3151f))
					.right(
						sge::renderer::vector3(-0.00920353f,0.0f,-0.999958f))
					.up(
						sge::renderer::vector3(0.982748f,0.184729f,-0.00904513f))
					.forward(
						sge::renderer::vector3(0.184722f,-0.982789f,-0.00170016f))));

	fcppt::signal::scoped_connection const console_cb(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::f1,
				std::tr1::bind(
					&toggle_console_active,
					fcppt::ref(
						console_gfx),
					fcppt::ref(
						camera)))));

	// Adapt the camera to the viewport
	fcppt::signal::scoped_connection const viewport_connection(
		sys.viewport_manager().manage_callback(
			std::tr1::bind(
				sge::camera::projection::update_perspective_from_viewport,
				fcppt::ref(
					sys.renderer()),
				fcppt::ref(
					camera),
				sge::renderer::projection::fov(
					fcppt::math::deg_to_rad(
						90.f)),
				sge::renderer::projection::near(
					0.1f),
				// Far plane
				sge::renderer::projection::far(
					1000.f))));

	flakelib::volume::smoke_simulation smoke_simulation(
		opencl_system.command_queue(),
		sys.renderer(),
		sys.image_system(),
		global_build_options,
		sge::parse::json::find_member_exn<sge::parse::json::object>(
			config_file.members,
			FCPPT_TEXT("smoke-simulation")));

	fcppt::signal::scoped_connection const wind_speed_cb(
		console_object.insert(
			sge::console::callback::from_functor<void(cl_float)>(
				std::tr1::bind(
					&flakelib::volume::smoke_simulation::external_force_magnitude,
					&smoke_simulation,
					std::tr1::placeholders::_1),
				sge::console::callback::name(
					SGE_FONT_TEXT_LIT("wind_speed")),
				sge::console::callback::short_description(
					SGE_FONT_TEXT_LIT("Usage: /wind_speed <speed>")))
				.long_description(
					SGE_FONT_TEXT_LIT("Changes wind speed"))));

	fcppt::signal::scoped_connection const arrows_cb(
		console_object.insert(
			sge::console::callback::from_functor<void()>(
				std::tr1::bind(
					&flakelib::volume::smoke_simulation::toggle_arrows,
					&smoke_simulation),
				sge::console::callback::name(
					SGE_FONT_TEXT_LIT("toggle_arrows")),
				sge::console::callback::short_description(
					SGE_FONT_TEXT_LIT("Usage: /toggle_arrows")))
				.long_description(
					SGE_FONT_TEXT_LIT("Toggle arrow drawing"))));

	sge::timer::basic<sge::timer::clocks::standard> camera_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			sge::camera::duration(
				1.0f)));

	sge::timer::basic<sge::timer::clocks::standard> revolve_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			fcppt::chrono::seconds(
				10)));

	sge::timer::basic<sge::timer::clocks::standard> delta_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			fcppt::chrono::seconds(1)));

	while(running)
	{
		sys.window_system().poll();

		flakelib::duration const current_delta =
			20.0f *
			sge::timer::elapsed_and_reset<flakelib::duration>(
				delta_timer);

		smoke_simulation.update(
			current_delta);

		camera.update(
			sge::timer::elapsed<sge::camera::duration>(
				camera_timer));

		/*
		std::cout << "camera gizmo pos: " << camera.gizmo().position() << "\n";
		std::cout << "camera gizmo right: " << camera.gizmo().right() << "\n";
		std::cout << "camera gizmo up: " << camera.gizmo().up() << "\n";
		std::cout << "camera gizmo forward: " << camera.gizmo().forward() << "\n";
		*/

		camera_timer.reset();

		sge::renderer::state::scoped scoped_global_state(
			sys.renderer(),
			sge::renderer::state::list
				(sge::renderer::state::bool_::clear_back_buffer = true)
				(sge::renderer::state::float_::depth_buffer_clear_val = 1.f)
				(sge::renderer::state::color::back_buffer_clear_color = sge::image::colors::black())
				(sge::renderer::state::bool_::clear_depth_buffer = true));

		sge::renderer::scoped_block const block_(
			sys.renderer());

		{
			sge::renderer::state::scoped scoped_local_state(
				sys.renderer(),
				sge::renderer::state::list
					(sge::renderer::state::bool_::enable_alpha_blending = true)
					(sge::renderer::state::cull_mode::off)
					(sge::renderer::state::depth_func::less)
					(sge::renderer::state::draw_mode::fill)
					(sge::renderer::state::stencil_func::off)
					(sge::renderer::state::source_blend_func::src_alpha)
					(sge::renderer::state::dest_blend_func::inv_src_alpha));

			smoke_simulation.render(
				camera.gizmo().position(),
				camera.mvp());
		}

		if(console_gfx.active())
			console_gfx.render();
	}
}
catch(
	fcppt::exception const &_error
)
{
	fcppt::io::cerr()
		<< _error.string()
		<< FCPPT_TEXT('\n');

	return EXIT_FAILURE;
}
catch(
	std::exception const &_error
)
{
	std::cerr
		<< _error.what()
		<< '\n';

	return EXIT_FAILURE;
}
