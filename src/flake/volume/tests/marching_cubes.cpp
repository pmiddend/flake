#include <sge/shader/activate_everything.hpp>
#include <sge/renderer/texture/filter/trilinear.hpp>
#include <sge/renderer/texture/filter/linear.hpp>
#include <sge/renderer/texture/filter/scoped.hpp>
#include <flake/catch_statements.hpp>
#include <flake/media_path_from_string.hpp>
#include <flake/volume/tests/marching_cubes.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/marching_cubes/vf/format.hpp>
#include <flakelib/splatter/box/object.hpp>
#include <flakelib/splatter/pen/object.hpp>
#include <flakelib/volume/retrieve_filled_float_buffer.hpp>
#include <sge/camera/coordinate_system/identity.hpp>
#include <sge/camera/first_person/parameters.hpp>
#include <sge/camera/matrix_conversion/world.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/image/colors.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/onscreen_target.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/clear/parameters.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/draw_mode.hpp>
#include <sge/renderer/state/float.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/texture/address_mode2.hpp>
#include <sge/renderer/texture/create_planar_from_path.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/set_address_mode2.hpp>
#include <sge/renderer/texture/volume.hpp>
#include <sge/renderer/texture/volume_shared_ptr.hpp>
#include <sge/renderer/texture/mipmap/all_levels.hpp>
#include <sge/shader/activate_bare.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/update_single_uniform.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/parameters.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/math/dim/object_impl.hpp>
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
		sge::parse::json::find_and_convert_member<sge::opencl::memory_object::dim3>(
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
		this->renderer(),
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
	boundary_buffer_(
		flakelib::volume::retrieve_filled_float_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get(),
			0.0f)),
	gradient_(
		this->program_context(),
		this->buffer_pool()),
	scan_(
		this->program_context(),
		this->buffer_pool()),
	marching_cubes_manager_(
		this->renderer(),
		scan_,
		gradient_,
		this->program_context()),
	shader_(
		sge::shader::object_parameters(
			this->renderer(),
			marching_cubes_manager_.vertex_declaration(),
			sge::shader::vf_to_string<flakelib::marching_cubes::vf::format>(),
			fcppt::assign::make_container<sge::shader::variable_sequence>
				(sge::shader::variable(
					"mvp",
					sge::shader::variable_type::uniform,
					sge::shader::matrix(
						sge::renderer::matrix4::identity(),
						sge::shader::matrix_flags::projection)))
				(sge::shader::variable(
					"texture_repeats",
					sge::shader::variable_type::constant,
					sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
						this->configuration(),
						sge::parse::json::string_to_path(
							FCPPT_TEXT("snow-texture-repeats")))))
				(sge::shader::variable(
					"sun_direction",
					sge::shader::variable_type::constant,
					sge::renderer::vector3(
						0.8804509f,
						0.17609018f,
						0.440225453f))),
			fcppt::assign::make_container<sge::shader::sampler_sequence>
				(sge::shader::sampler(
					"snow_rough",
					sge::shader::texture_variant(
						sge::renderer::texture::planar_shared_ptr(
							sge::renderer::texture::create_planar_from_path(
								flake::media_path_from_string(
									FCPPT_TEXT("textures/snow_rough.png")),
								this->renderer(),
								this->image_system(),
								sge::renderer::texture::mipmap::all_levels(
									sge::renderer::texture::mipmap::auto_generate::yes),
								sge::renderer::resource_flags::none)))))
				(sge::shader::sampler(
					"snow_calm",
					sge::shader::texture_variant(
						sge::renderer::texture::planar_shared_ptr(
							sge::renderer::texture::create_planar_from_path(
								flake::media_path_from_string(
									FCPPT_TEXT("textures/snow_calm.png")),
								this->renderer(),
								this->image_system(),
								sge::renderer::texture::mipmap::all_levels(
									sge::renderer::texture::mipmap::auto_generate::yes),
								sge::renderer::resource_flags::none))))))
			.vertex_shader(
				flake::media_path_from_string(
					FCPPT_TEXT("shaders/marching_cubes/vertex.glsl")))
			.fragment_shader(
				flake::media_path_from_string(
					FCPPT_TEXT("shaders/marching_cubes/fragment.glsl")))
			.name(
				FCPPT_TEXT("Marching cubes"))),
	marching_cubes_(
		this->buffer_pool(),
		marching_cubes_manager_,
		flakelib::marching_cubes::grid_size(
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
	delta_timer_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			boost::chrono::seconds(1)))
{
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
						20,
						20,
						20))),
			flakelib::splatter::pen::is_round(
				true),
			flakelib::splatter::pen::is_smooth(
				true),
			flakelib::splatter::pen::draw_mode::mix,
			flakelib::splatter::pen::blend_factor(
				1.0f)),
		static_cast<cl_float>(
			1.0f));

	marching_cubes_.update(
		flakelib::marching_cubes::density_view(
			boundary_buffer_->value()));
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
flake::volume::tests::marching_cubes::render()
{
	sge::renderer::state::scoped scoped_state(
		this->renderer(),
		sge::renderer::state::list
			(this->feature_active(test::json_identifier(FCPPT_TEXT("wireframe")))
			?
				 sge::renderer::state::draw_mode::line
			:
				sge::renderer::state::draw_mode::fill));

	this->renderer().onscreen_target().clear(
		sge::renderer::clear::parameters()
			.back_buffer(
				sge::image::colors::black())
			.depth_buffer(
				sge::renderer::clear::depth_buffer_value(
					1.0f)));

	sge::shader::scoped scoped_shader(
		shader_,
		sge::shader::activate_everything());

	shader_.update_uniform(
		"mvp",
		sge::shader::matrix(
			sge::camera::matrix_conversion::world_projection(
				camera_.coordinate_system(),
				camera_.projection_matrix()),
			sge::shader::matrix_flags::projection));

	sge::renderer::texture::set_address_mode2(
		this->renderer(),
		sge::renderer::texture::stage(
			0u),
		sge::renderer::texture::address_mode2(
			sge::renderer::texture::address_mode::repeat));

	sge::renderer::texture::set_address_mode2(
		this->renderer(),
		sge::renderer::texture::stage(
			1u),
		sge::renderer::texture::address_mode2(
			sge::renderer::texture::address_mode::repeat));

	sge::renderer::texture::filter::scoped scoped_texture_filter0(
		this->renderer(),
		sge::renderer::texture::stage(
			0u),
		sge::renderer::texture::filter::linear());

	sge::renderer::texture::filter::scoped scoped_texture_filter1(
		this->renderer(),
		sge::renderer::texture::stage(
			1u),
		sge::renderer::texture::filter::linear());

	marching_cubes_manager_.render();

	test::base::render();
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

	if(this->feature_active(flake::test::json_identifier(FCPPT_TEXT("frameupdate"))))
		marching_cubes_.update(
			flakelib::marching_cubes::density_view(
				boundary_buffer_->value()));
}
