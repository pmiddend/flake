#include <flakelib/buffer/volume_view.hpp>
#include <sge/opencl/memory_object/size_type.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <sge/shader/object_parameters.hpp>
#include <fcppt/assign/make_array.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/opencl/single_device_system/parameters.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <fcppt/to_std_string.hpp>
#include <flakelib/build_options.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <sge/camera/projection/object.hpp>
#include <sge/camera/projection/update_perspective_from_viewport.hpp>
#include <sge/camera/spherical/movement_speed.hpp>
#include <sge/camera/spherical/object.hpp>
#include <sge/camera/spherical/parameters.hpp>
#include <sge/config/media_path.hpp>
#include <sge/font/metrics_ptr.hpp>
#include <sge/font/rect.hpp>
#include <sge/font/size_type.hpp>
#include <sge/font/system.hpp>
#include <sge/font/text/align_h.hpp>
#include <sge/font/text/align_v.hpp>
#include <sge/font/text/draw.hpp>
#include <sge/font/text/drawer_3d.hpp>
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
#include <sge/renderer/active_target.hpp>
#include <sge/renderer/depth_stencil_buffer.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/first_vertex.hpp>
#include <sge/renderer/lock_mode.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/renderer/no_multi_sampling.hpp>
#include <sge/renderer/nonindexed_primitive_type.hpp>
#include <sge/renderer/parameters.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/scoped_block.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/target_base.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_count.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/renderer/viewport.hpp>
#include <sge/renderer/visual_depth.hpp>
#include <sge/renderer/vsync.hpp>
#include <sge/renderer/projection/far.hpp>
#include <sge/renderer/projection/fov.hpp>
#include <sge/renderer/projection/near.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/draw_mode.hpp>
#include <sge/renderer/state/float.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/stencil_func.hpp>
#include <sge/renderer/texture/create_planar_from_path.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
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
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/dynamic/make_part_index.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/activation_method.hpp>
#include <sge/shader/matrix.hpp>
#include <sge/shader/matrix_flags.hpp>
#include <sge/shader/object.hpp>
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
#include <sge/viewport/fill_on_resize.hpp>
#include <sge/viewport/manager.hpp>
#include <sge/window/dim.hpp>
#include <sge/window/parameters.hpp>
#include <sge/window/system.hpp>
#include <sge/window/title.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/chrono/seconds.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/log/activate_levels.hpp>
#include <fcppt/log/level.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/math/twopi.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/matrix/basic_impl.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/tr1/functional.hpp>
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
	sge::systems::instance const sys(
		sge::systems::list()
			(sge::systems::window(
				sge::window::parameters(
					sge::window::title(
						FCPPT_TEXT("sge test for sge::shader::object")),
					sge::window::dim(1024,768))))
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

	sge::camera::spherical::object camera(
		sge::camera::spherical::parameters(
			// Movement speed.
			sge::camera::spherical::movement_speed(
				2.0f),
			// min_radius
			0.5f,
			sys.keyboard_collector())
			.radius(
				3.f)
			.damping(
				0.97f));

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

	sge::opencl::memory_object::buffer cl_buffer(
		opencl_system.context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::byte_size(
			grid_size * grid_size * grid_size * 4 * sizeof(cl_float)));

	sge::opencl::memory_object::buffer gl_buffer(
		opencl_system.context(),
		*vb,
		sge::opencl::memory_object::renderer_buffer_lock_mode::read_write);

	sge::opencl::program::object fill_program(
		opencl_system.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/fill_with_arrows.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					global_build_options.get())));

	sge::opencl::kernel::object fill_program_kernel(
		fill_program,
		sge::opencl::kernel::name(
			"fill_with_arrows"));

	fill_program_kernel.argument(
		sge::opencl::kernel::argument_index(
			0),
		cl_buffer);

	sge::opencl::command_queue::enqueue_kernel(
		opencl_system.command_queue(),
		fill_program_kernel,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(cl_buffer.byte_size() / 4 / 4).container());

	conversion_object.to_arrow_vb(
		flakelib::volume::conversion::cl_buffer(
			flakelib::buffer::volume_view<cl_float4>(
				cl_buffer,
				sge::opencl::memory_object::dim3(
					grid_size,
					grid_size,
					grid_size))),
		flakelib::volume::conversion::gl_buffer(
			gl_buffer),
		flakelib::volume::conversion::arrow_scale(
			1.0f),
		flakelib::volume::conversion::grid_scale(
			1.0f));

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

	while(running)
	{
		sys.window_system().poll();

		camera.update(
			sge::timer::elapsed<sge::camera::duration>(
				camera_timer));

		camera_timer.reset();

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
