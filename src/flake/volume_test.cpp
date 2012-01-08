#include <flakelib/volume/boundary/object.hpp>
#include <flakelib/volume/simulation/stam/object.hpp>
#include <sge/parse/json/array.hpp>
#include <flakelib/utf8_file_to_fcppt_string.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/parse_string_exn.hpp>
#include <sge/parse/json/path.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/parse/json/config/create_command_line_parameters.hpp>
#include <sge/parse/json/config/merge_command_line_parameters.hpp>
#include <flakelib/utility/object.hpp>
#include <flakelib/volume/laplace_solver/jacobi.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/assign/make_container.hpp>
#include <sge/timer/basic.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/parameters.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <fcppt/chrono/seconds.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <fcppt/io/cerr.hpp>
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
#include <flakelib/buffer/volume_view.hpp>
#include <flakelib/build_options.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <sge/camera/first_person/object.hpp>
#include <sge/camera/first_person/parameters.hpp>
#include <sge/camera/projection/object.hpp>
#include <sge/camera/projection/update_perspective_from_viewport.hpp>
#include <sge/config/media_path.hpp>
#include <sge/font/metrics_ptr.hpp>
#include <sge/font/rect.hpp>
#include <fcppt/io/cout.hpp>
#include <sge/font/size_type.hpp>
#include <sge/font/system.hpp>
#include <sge/font/text/align_h.hpp>
#include <sge/font/text/align_v.hpp>
#include <sge/font/text/drawer_3d.hpp>
#include <sge/font/text/draw.hpp>
#include <sge/font/text/flags_none.hpp>
#include <sge/font/text/lit.hpp>
#include <sge/font/text/part.hpp>
#include <sge/font/text/string.hpp>
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
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/draw_mode.hpp>
#include <sge/renderer/state/float.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/stencil_func.hpp>
#include <sge/renderer/target_base.hpp>
#include <sge/renderer/texture/create_planar_from_path.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_count.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/dynamic/make_part_index.hpp>
#include <sge/renderer/vf/format.hpp>
#include <sge/renderer/vf/iterator.hpp>
#include <sge/renderer/vf/make_unspecified_tag.hpp>
#include <sge/renderer/vf/part.hpp>
#include <sge/renderer/vf/pos.hpp>
#include <sge/renderer/vf/texpos.hpp>
#include <sge/renderer/vf/unspecified.hpp>
#include <sge/renderer/vf/vector.hpp>
#include <sge/renderer/vf/vertex.hpp>
#include <sge/renderer/vf/view.hpp>
#include <sge/renderer/viewport.hpp>
#include <sge/renderer/visual_depth.hpp>
#include <sge/renderer/vsync.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/activation_method.hpp>
#include <sge/shader/matrix_flags.hpp>
#include <sge/shader/matrix.hpp>
#include <sge/shader/object.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/sampler_sequence.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/update_single_uniform.hpp>
#include <sge/shader/variable.hpp>
#include <sge/shader/variable_sequence.hpp>
#include <sge/shader/variable_type.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <sge/systems/cursor_option_field.hpp>
#include <sge/systems/font.hpp>
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
namespace vf
{
namespace tags
{
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(position);
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(color);
}

typedef
sge::renderer::vf::unspecified
<
	sge::renderer::vf::vector
	<
		sge::renderer::scalar,
		4
	>,
	tags::position
>
position;

typedef
sge::renderer::vf::unspecified
<
	sge::renderer::vf::vector
	<
		sge::renderer::scalar,
		4
	>,
	tags::color
>
color;

typedef
sge::renderer::vf::part
<
	boost::mpl::vector2
	<
		position,
		color
	>
>
format_part;

typedef
sge::renderer::vf::format
<
	boost::mpl::vector1<format_part>
>
format;

typedef
sge::renderer::vf::view<format_part>
format_part_view;
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
			(sge::systems::input(
				sge::systems::input_helper_field(
					sge::systems::input_helper::keyboard_collector) | sge::systems::input_helper::mouse_collector,
				sge::systems::cursor_option_field(
					sge::systems::cursor_option::exclusive))));

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
				4.f),
			sge::camera::first_person::rotation_speed(
				200.f),
			sys.keyboard_collector(),
			sys.mouse_collector()));

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

	sge::renderer::vertex_declaration_ptr const vertex_declaration(
		sys.renderer().create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<vf::format>()));

	sge::shader::object shader(
		sge::shader::object_parameters(
			sys.renderer(),
			*vertex_declaration,
			sge::shader::vf_to_string<vf::format>(),
			fcppt::assign::make_container<sge::shader::variable_sequence>
				(sge::shader::variable(
					"mvp",
					sge::shader::variable_type::uniform,
					sge::shader::matrix(
						sge::renderer::matrix4::identity(),
						sge::shader::matrix_flags::projection)))
				(sge::shader::variable(
					"initial_position",
					sge::shader::variable_type::uniform,
					sge::renderer::vector3(
						0.0f,
						0.0f,
						0.0f))),
			sge::shader::sampler_sequence())
			// We can specify more than one fragment and vertex shader here
			.vertex_shader(
				flakelib::media_path_from_string(
					FCPPT_TEXT("shaders/volume_arrow/vertex.glsl")))
			.fragment_shader(
				flakelib::media_path_from_string(
					FCPPT_TEXT("shaders/volume_arrow/fragment.glsl")))
			// The name is displayed in an error or warning message from the shader.
			.name(
				FCPPT_TEXT("Main shader")));

	unsigned const grid_size =
		64u;

	flakelib::volume::conversion::object conversion_object(
		opencl_system.command_queue(),
		global_build_options);

	sge::renderer::vertex_buffer_ptr const vb(
		sys.renderer().create_vertex_buffer(
			*vertex_declaration,
			sge::renderer::vf::dynamic::make_part_index
			<
				vf::format,
				vf::format_part
			>(),
			// 6 sides, 2 triangles per side, 3 vertices (we don't use an
			// index buffer - overkill)
			sge::renderer::vertex_count(
				grid_size * grid_size * grid_size * 2u),
			sge::renderer::resource_flags::none));

	sge::opencl::memory_object::buffer gl_buffer(
		opencl_system.context(),
		*vb,
		sge::opencl::memory_object::renderer_buffer_lock_mode::read_write);

	flakelib::buffer_pool::object buffer_pool(
		opencl_system.context());

	flakelib::utility::object utility_object(
		opencl_system.command_queue(),
		global_build_options);

	flakelib::volume::boundary::object boundary_object(
		opencl_system.command_queue(),
		utility_object,
		buffer_pool,
		global_build_options,
		sge::opencl::memory_object::dim3(
			grid_size,
			grid_size,
			grid_size));

	/*
	boundary_object.add_cube(
		flakelib::volume::boundary::cube_position(
			sge::opencl::memory_object::dim3(
				0,
				0,
				0)),
		flakelib::volume::boundary::cube_width(
			grid_size/8));
			*/

	boundary_object.add_sphere(
		flakelib::volume::boundary::sphere_center(
			sge::opencl::memory_object::dim3(
				grid_size/2,
				grid_size/2,
				grid_size/2)),
		flakelib::volume::boundary::radius(
			grid_size/3));

	/*
	boundary_object.add_sphere(
		flakelib::volume::boundary::sphere_center(
			sge::opencl::memory_object::dim3(
				grid_size - grid_size/10,
				grid_size - grid_size/10,
				grid_size - grid_size/10)),
		flakelib::volume::boundary::radius(
			grid_size/5));
	*/


	flakelib::volume::laplace_solver::jacobi jacobi_solver(
		buffer_pool,
		opencl_system.command_queue(),
		global_build_options,
		flakelib::volume::laplace_solver::boundary(
			boundary_object.get()),
		flakelib::volume::laplace_solver::grid_scale(
			1.0f),
		flakelib::volume::laplace_solver::iterations(
			51));

	flakelib::volume::simulation::stam::object stam_simulation(
		opencl_system.command_queue(),
		flakelib::volume::boundary::view(
			boundary_object.get()),
		sge::parse::json::find_and_convert_member<sge::parse::json::object>(
			config_file,
			sge::parse::json::path(
				FCPPT_TEXT("stam-volume-test"))),
		global_build_options,
		buffer_pool,
		utility_object,
		jacobi_solver);


	// Some render states
	sys.renderer().state(
		sge::renderer::state::list
			(sge::renderer::state::bool_::clear_back_buffer = true)
			(sge::renderer::state::bool_::clear_depth_buffer = true)
			(sge::renderer::state::bool_::enable_alpha_blending = false)
			(sge::renderer::state::cull_mode::off)
			(sge::renderer::state::depth_func::less)
			(sge::renderer::state::draw_mode::fill)
			(sge::renderer::state::float_::depth_buffer_clear_val = 1.f)
			(sge::renderer::state::stencil_func::off)
			(sge::renderer::state::color::back_buffer_clear_color = sge::image::colors::black()));

	sge::timer::basic<sge::timer::clocks::standard> camera_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			sge::camera::duration(
				1.0f)));

	sge::timer::basic<sge::timer::clocks::standard> revolve_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			fcppt::chrono::seconds(
				10)));

	sge::font::text::drawer_3d font_drawer(
		sys.renderer(),
		sge::image::colors::red());

	sge::font::metrics_ptr const font_metrics(
		sys.font_system().create_font(
			sge::config::media_path()
				/ FCPPT_TEXT("fonts")
				/ FCPPT_TEXT("default.ttf"),
			static_cast<sge::font::size_type>(
				30)));

	sge::timer::basic<sge::timer::clocks::standard> delta_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			fcppt::chrono::seconds(1)));

	flakelib::duration delta(0.0f);

	while(running)
	{
		sys.window_system().poll();

		delta +=
			1.0f * sge::timer::elapsed_and_reset<flakelib::duration>(
				delta_timer);

		{
			stam_simulation.update(
				delta);

			delta = flakelib::duration(0.0f);
		}

		camera.update(
			sge::timer::elapsed<sge::camera::duration>(
				camera_timer));

		camera_timer.reset();

		conversion_object.to_arrow_vb(
			flakelib::volume::conversion::cl_buffer(
				stam_simulation.velocity()),
			flakelib::volume::conversion::gl_buffer(
				gl_buffer),
			flakelib::volume::conversion::arrow_scale(
				sge::parse::json::find_and_convert_member<cl_float>(
					config_file,
					sge::parse::json::path(
						FCPPT_TEXT("stam-volume-test")) / FCPPT_TEXT("arrow-scale"))),
			flakelib::volume::conversion::grid_scale(
				sge::parse::json::find_and_convert_member<cl_float>(
					config_file,
					sge::parse::json::path(
						FCPPT_TEXT("stam-volume-test")) / FCPPT_TEXT("grid-scale"))));

		sge::renderer::scoped_block const block_(
			sys.renderer());

		{
			sge::shader::scoped scoped_shader(
				shader,
				sge::shader::activate_everything());

			sge::renderer::scoped_vertex_buffer const vb_context(
				sys.renderer(),
				*vb);

			shader.update_uniform(
				"mvp",
				sge::shader::matrix(
					camera.mvp(),
					sge::shader::matrix_flags::projection));

			sys.renderer().render_nonindexed(
				sge::renderer::first_vertex(0),
				sge::renderer::vertex_count(
					vb->size()),
				sge::renderer::nonindexed_primitive_type::line);
		}

		/*
		sge::font::text::draw(
			*font_metrics,
			font_drawer,
			SGE_FONT_TEXT_LIT("Press 'e' to toggle bump mapping"),
			sge::font::rect(
				sge::font::rect::vector::null(),
				fcppt::math::dim::structure_cast<sge::font::rect::dim>(
					sge::renderer::active_target(
						sys.renderer()
					).viewport().get().size()
				)
			),
			sge::font::text::align_h::left,
			sge::font::text::align_v::top,
			sge::font::text::flags::none);
			*/
	}

	fcppt::io::cout() << stam_simulation.parent_profiler() << FCPPT_TEXT("\n");
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
