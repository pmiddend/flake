#include <flake/catch_statements.hpp>
#include <sge/renderer/state/core/rasterizer/object_scoped_ptr.hpp>
#include <sge/renderer/state/core/rasterizer/object.hpp>
#include <sge/renderer/state/core/rasterizer/scoped.hpp>
#include <sge/renderer/state/core/rasterizer/parameters.hpp>
#include <flake/media_path_from_string.hpp>
#include <flake/volume/snow_cover/scoped.hpp>
#include <flake/volume/tests/marching_cubes.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/marching_cubes/vf/format.hpp>
#include <flakelib/splatter/box/object.hpp>
#include <flakelib/splatter/pen/object.hpp>
#include <flakelib/timer/object.hpp>
#include <flakelib/volume/retrieve_filled_float_buffer.hpp>
#include <sge/camera/coordinate_system/identity.hpp>
#include <sge/camera/first_person/parameters.hpp>
#include <sge/camera/matrix_conversion/world.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/image/colors.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/device/ffp.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/clear/parameters.hpp>
#include <sge/renderer/context/ffp.hpp>
#include <sge/renderer/texture/create_planar_from_path.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/mipmap/all_levels.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/parameters.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/chrono.hpp>
#include <fcppt/config/external_end.hpp>


awl::main::exit_code const
custom_main(
	awl::main::function_context const &);

awl::main::exit_code const
custom_main(
	awl::main::function_context const &_function_context)
try
{
	flake::volume::tests::marching_cubes s(
		_function_context);

	return
		s.run();
}
FLAKE_CATCH_STATEMENTS

flake::volume::tests::marching_cubes::marching_cubes(
	awl::main::function_context const &_function_context)
:
	flake::test::base(
		_function_context,
		sge::window::title(
			FCPPT_TEXT("marching_cubes test")),
		flake::test::json_identifier(
			FCPPT_TEXT("marching-cubes")),
		fcppt::assign::make_container<flake::test::feature_sequence>
			(flake::test::feature(
			 	flake::test::json_identifier(
					FCPPT_TEXT("wireframe")),
				flake::test::optional_key_code(
					sge::input::keyboard::key_code::f2)))
			(flake::test::feature(
			 	flake::test::json_identifier(
					FCPPT_TEXT("frameupdate")),
				flake::test::optional_key_code(
					sge::input::keyboard::key_code::f3))),
		sge::systems::cursor_option_field(
			sge::systems::cursor_option::exclusive)),
	simulation_size_(
		sge::parse::json::find_and_convert_member<sge::opencl::dim3>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("simulation-size")))),
	camera_(
		sge::camera::first_person::parameters(
			this->keyboard(),
			this->mouse(),
			sge::camera::first_person::is_active(
				true),
			sge::camera::first_person::movement_speed(
				1.0f),
			sge::camera::coordinate_system::identity())),
	perspective_projection_from_viewport_(
		camera_,
		this->viewport_manager(),
		sge::renderer::projection::near(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("near-plane")))),
		sge::renderer::projection::far(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("far-plane")))),
		sge::renderer::projection::fov(
			fcppt::math::deg_to_rad(
				sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
					this->configuration(),
					sge::parse::json::string_to_path(
						FCPPT_TEXT("fov")))))),
	fill_buffer_(
		this->program_context()),
	splatter_(
		this->program_context()),
	conversion_(
		this->program_context()),
	boundary_buffer_(
		conversion_.binvox_file_to_buffer(
			this->buffer_pool(),
			flake::media_path_from_string(
				FCPPT_TEXT("scenes/")+
				sge::parse::json::find_and_convert_member<fcppt::string>(
					this->configuration(),
					sge::parse::json::string_to_path(
						FCPPT_TEXT("scene-name")))+
				FCPPT_TEXT("/scene.binvox")),
			flakelib::volume::conversion::optional_height(
				simulation_size_.get().h()))
		/*
		flakelib::volume::retrieve_filled_float_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get(),
			0.0f)*/),
	gradient_(
		this->program_context(),
		this->buffer_pool()),
	scan_(
		this->program_context(),
		this->buffer_pool()),
	marching_cubes_manager_(
		this->renderer(),
		flakelib::marching_cubes::cpu::grid_size(
			sge::renderer::dim3(
				static_cast<sge::renderer::size_type>(
					simulation_size_.get().w()),
				static_cast<sge::renderer::size_type>(
					simulation_size_.get().h()),
				static_cast<sge::renderer::size_type>(
					simulation_size_.get().d()))),
		flakelib::marching_cubes::iso_level(
			0.4f)),
/*
	marching_cubes_manager_(
		this->renderer(),
		scan_,
		gradient_,
		this->program_context(),
		this->buffer_pool()),
*/
	snow_cover_(
		camera_,
		this->renderer(),
		marching_cubes_manager_.vertex_declaration(),
		this->shader_context(),
		flake::volume::snow_cover::texture_repeats(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("snow-cover/texture-repeats")))),
		flake::volume::snow_cover::steep_texture(
			sge::renderer::texture::planar_shared_ptr(
				sge::renderer::texture::create_planar_from_path(
					flake::media_path_from_string(
						FCPPT_TEXT("textures/")+
						sge::parse::json::find_and_convert_member<fcppt::string>(
							this->configuration(),
							sge::parse::json::string_to_path(
								FCPPT_TEXT("snow-cover/steep-texture")))+
						FCPPT_TEXT(".png")),
					this->renderer(),
					this->image_system(),
					sge::renderer::texture::mipmap::all_levels(
						sge::renderer::texture::mipmap::auto_generate::yes),
					sge::renderer::resource_flags_field::null()))),
		flake::volume::snow_cover::flat_texture(
			sge::renderer::texture::planar_shared_ptr(
				sge::renderer::texture::create_planar_from_path(
					flake::media_path_from_string(
						FCPPT_TEXT("textures/")+
						sge::parse::json::find_and_convert_member<fcppt::string>(
							this->configuration(),
							sge::parse::json::string_to_path(
								FCPPT_TEXT("snow-cover/flat-texture")))+
						FCPPT_TEXT(".png")),
					this->renderer(),
					this->image_system(),
					sge::renderer::texture::mipmap::all_levels(
						sge::renderer::texture::mipmap::auto_generate::yes),
					sge::renderer::resource_flags_field::null()))),
		flake::volume::snow_cover::sun_direction(
			sge::parse::json::find_and_convert_member<sge::renderer::vector3>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("sun-direction"))))),
/*
	marching_cubes_(
		this->buffer_pool(),
		marching_cubes_manager_,
		flakelib::marching_cubes::gpu::grid_size(
			flakelib::cl::uint4(
				static_cast<cl_uint>(
					simulation_size_.get().w()),
				static_cast<cl_uint>(
					simulation_size_.get().h()),
				static_cast<cl_uint>(
					simulation_size_.get().d()),
				0u)),
		flakelib::marching_cubes::iso_level(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("iso-level"))))),
*/
	delta_timer_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			boost::chrono::seconds(
				1)))
{
	/*
	splatter_.splat_volume_float(
		boundary_buffer_->value(),
		flakelib::splatter::pen::volume(
			flakelib::splatter::box::object(
				flakelib::splatter::box::position(
					flakelib::splatter::box::position::value_type(
						10,
						10,
						10)),
				flakelib::splatter::box::size(
					flakelib::splatter::box::size::value_type(
						22,
						22,
						22))),
			flakelib::splatter::pen::is_round(
				true),
			flakelib::splatter::pen::is_smooth(
				true),
			flakelib::splatter::pen::draw_mode::mix,
			flakelib::splatter::pen::blend_factor(
				1.0f)),
		static_cast<cl_float>(
			1.0f));
	*/

	{
		flakelib::timer::object t(
			std::cout,
			"marching cubes");

		marching_cubes_manager_.construct_from_cl_buffer(
			this->opencl_system().command_queue(),
			boundary_buffer_->value());

		marching_cubes_manager_.run();

		marching_cubes_manager_.update_buffers();
	}
	/*
	marching_cubes_.update(
		flakelib::marching_cubes::gpu::density_view(
			boundary_buffer_->value()));
	*/
}

awl::main::exit_code const
flake::volume::tests::marching_cubes::run()
{
	return
		flake::test::base::run();
}

flake::volume::tests::marching_cubes::~marching_cubes()
{
}

void
flake::volume::tests::marching_cubes::render(
	sge::renderer::context::ffp &_context)
{
	sge::renderer::state::core::rasterizer::object_scoped_ptr const rasterizer_state(
		this->renderer().create_rasterizer_state(
			sge::renderer::state::core::rasterizer::parameters(
				sge::renderer::state::core::rasterizer::cull_mode::off,
				this->feature_active(test::json_identifier(FCPPT_TEXT("wireframe")))
				?
					sge::renderer::state::core::rasterizer::fill_mode::line
				:
					sge::renderer::state::core::rasterizer::fill_mode::solid,
				sge::renderer::state::core::rasterizer::enable_scissor_test(
					false))));

	sge::renderer::state::core::rasterizer::scoped scoped_rasterizer_state(
		_context,
		*rasterizer_state);

	_context.clear(
		sge::renderer::clear::parameters()
			.back_buffer(
				sge::image::colors::black())
			.depth_buffer(
				sge::renderer::clear::depth_buffer_value(
					1.0f)));

	flake::volume::snow_cover::scoped scoped_snow_cover(
		snow_cover_,
		_context);

	marching_cubes_manager_.render(
		_context);
}

void
flake::volume::tests::marching_cubes::update()
{
	test::base::update();

	flakelib::duration const
		raw_delta =
			sge::timer::elapsed_and_reset<flakelib::duration>(
				delta_timer_),
		delta =
			boost::rational_cast<flakelib::duration::rep>(
				this->current_multiplier().get()) *
			raw_delta;

	camera_.update(
		10.0f * raw_delta);

	/*
	if(this->feature_active(flake::test::json_identifier(FCPPT_TEXT("frameupdate"))))
		marching_cubes_.update(
			flakelib::marching_cubes::gpu::density_view(
				boundary_buffer_->value()));
	*/
}
