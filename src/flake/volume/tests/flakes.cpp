#include <flake/catch_statements.hpp>
#include <flake/media_path_from_string.hpp>
#include <flake/test/information/string_conversion_adapter.hpp>
#include <flake/volume/snow_cover/scoped.hpp>
#include <flake/volume/tests/flakes.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/marching_cubes/vf/format.hpp>
#include <flakelib/volume/retrieve_filled_float_buffer.hpp>
#include <flakelib/volume/retrieve_zero_float4_buffer.hpp>
#include <sge/camera/coordinate_system/identity.hpp>
#include <sge/camera/first_person/parameters.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/image/colors.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/clear/parameters.hpp>
#include <sge/renderer/context/object.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/draw_mode.hpp>
#include <sge/renderer/state/float.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/target/onscreen.hpp>
#include <sge/renderer/texture/address_mode2.hpp>
#include <sge/renderer/texture/create_planar_from_path.hpp>
#include <sge/renderer/texture/set_address_mode2.hpp>
#include <sge/renderer/texture/volume.hpp>
#include <sge/renderer/texture/filter/scoped.hpp>
#include <sge/renderer/texture/filter/trilinear.hpp>
#include <sge/renderer/texture/mipmap/all_levels.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/parameters.hpp>
#include <sge/timer/reset_when_expired.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <fcppt/tr1/functional.hpp>
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
	flake::volume::tests::flakes s(
		_function_context);

	return
		s.run();
}
FLAKE_CATCH_STATEMENTS

flake::volume::tests::flakes::flakes(
	awl::main::function_context const &_function_context)
:
	flake::test::base(
		_function_context,
		sge::window::title(
			FCPPT_TEXT("Flakes test")),
		flake::test::json_identifier(
			FCPPT_TEXT("flakes")),
		fcppt::assign::make_container<test::feature_sequence>
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("arrows")),
				test::optional_key_code(
					sge::input::keyboard::key_code::f1)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("snowcover")),
				test::optional_key_code(
					sge::input::keyboard::key_code::f3)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("models")),
				test::optional_key_code(
					sge::input::keyboard::key_code::f4)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("wireframe")),
				test::optional_key_code(
					sge::input::keyboard::key_code::f5)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("marchingcubes")),
				test::optional_key_code(
					sge::input::keyboard::key_code::f6)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("flakes")),
				test::optional_key_code(
					sge::input::keyboard::key_code::f2))),
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
	conversion_object_(
		this->program_context()),
	arrows_manager_(
		this->renderer(),
		this->shader_context(),
		camera_),
	velocity_arrows_(
		this->opencl_system().context(),
		arrows_manager_,
		conversion_object_,
		flakelib::volume::conversion::arrow_scale(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("arrow-scale")))),
		flakelib::volume::conversion::grid_scale(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("grid-scale")))),
		flakelib::volume::conversion::origin(
			sge::renderer::vector3::null()),
		simulation_size_),
	wind_source_(
		this->program_context(),
		flakelib::volume::simulation::stam::wind_strength(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("wind-strength-start"))))),
	outflow_boundaries_(
		this->program_context()),
	semilagrangian_advection_(
		this->program_context(),
		this->buffer_pool()),
	divergence_(
		this->program_context(),
		this->buffer_pool()),
	jacobi_(
		this->program_context(),
		this->buffer_pool(),
		flakelib::volume::simulation::stam::iterations(
			sge::parse::json::find_and_convert_member<flakelib::volume::simulation::stam::iterations::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("jacobi-iterations"))))),
	subtract_pressure_gradient_(
		this->program_context()),
	boundary_buffer_(
		conversion_object_.raw_voxel_file_to_buffer(
			this->buffer_pool(),
			flake::media_path_from_string(
				FCPPT_TEXT("voxelized_models/")+
				sge::parse::json::find_and_convert_member<fcppt::string>(
					this->configuration(),
					sge::parse::json::string_to_path(
						FCPPT_TEXT("voxel-file/name")))),
			flakelib::volume::conversion::raw_voxel_file_dimension(
				sge::parse::json::find_and_convert_member<sge::opencl::size_type>(
					this->configuration(),
					sge::parse::json::string_to_path(
						FCPPT_TEXT("voxel-file/size")))))
		/*
		flakelib::volume::retrieve_filled_float_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get(),
			0.0f)*/),
	snow_density_buffer_(
		flakelib::volume::retrieve_filled_float_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get(),
			0.0f)),
	activity_buffer_(
		flakelib::volume::retrieve_filled_float_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get(),
			0.0f)),
	initial_guess_buffer_(
		flakelib::volume::retrieve_filled_float_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get(),
			0.0f)),
	velocity_buffer_(
		flakelib::volume::retrieve_zero_float4_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get())),
	flakes_(
		this->renderer(),
		this->shader_context(),
		camera_,
		this->opencl_system().context(),
		this->image_system(),
		flake::volume::flakes::count(
			sge::parse::json::find_and_convert_member<flake::volume::flakes::count::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("flakes/count")))),
		flake::volume::flakes::minimum_size(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("flakes/minimum-size")))),
		flake::volume::flakes::maximum_size(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("flakes/maximum-size")))),
		flake::volume::flakes::texture(
			sge::parse::json::find_and_convert_member<fcppt::string>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("flakes/texture")))),
		flake::volume::flakes::texture_tile_size(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("flakes/texture-tile-size")))),
		flake::volume::flakes::texture_tile_count(
			sge::parse::json::find_and_convert_member<flake::volume::flakes::texture_tile_count::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("flakes/texture-tile-count")))),
		simulation_size_),
	flakes_mover_(
		this->program_context(),
		this->buffer_pool(),
		flakes_.cl_positions(),
		flakes_.cl_point_sizes(),
		volume::flakes::snow_density_view(
			snow_density_buffer_->value()),
		volume::flakes::collision_increment(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("collision-increment")))),
		volume::flakes::activity_view(
			activity_buffer_->value()),
		flakes_.minimum_size(),
		flakes_.maximum_size(),
		volume::flakes::gravity_magnitude(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("gravity-magnitude")))),
		flakelib::marching_cubes::iso_level(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("iso-level"))))),
	models_(
		this->renderer(),
		this->shader_context(),
		this->image_system(),
		camera_,
		flake::volume::model::sun_direction(
			sge::parse::json::find_and_convert_member<sge::renderer::vector3>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("sun-direction"))))),
	model_(
		models_,
		flake::volume::model::identifier(
			sge::parse::json::find_and_convert_member<fcppt::string>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("voxel-file/model")))),
		flake::volume::model::position(
			sge::renderer::vector3(
				0.0f,
				0.0f,
				0.0f))),
	/*
	obstacles_(
		models_,
		sge::parse::json::find_and_convert_member<sge::parse::json::array>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("obstacles"))),
		flakelib::volume::boundary_buffer_view(
			boundary_buffer_->value()),
		splatter_),
		*/
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
		this->program_context(),
		this->buffer_pool()),
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
					sge::renderer::resource_flags::none))),
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
					sge::renderer::resource_flags::none))),
		flake::volume::snow_cover::sun_direction(
			sge::parse::json::find_and_convert_member<sge::renderer::vector3>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("sun-direction"))))),
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
	wind_strength_modulator_(
		std::tr1::bind(
			&flakelib::volume::simulation::stam::wind_source::wind_strength,
			&wind_source_,
			std::tr1::placeholders::_1),
		flakelib::value_modulator::minimum(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("wind-strength-minimum")))),
		flakelib::value_modulator::maximum(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("wind-strength-maximum")))),
		flakelib::value_modulator::frequency(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("wind-strength-frequency-seconds"))))),
	delta_timer_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			boost::chrono::seconds(1))),
	snow_cover_update_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			boost::chrono::seconds(1))),
	snow_cover_vertices_(
		0u),
	snow_cover_vertices_information_(
		this->information_manager(),
		test::information::identifier(
			FCPPT_TEXT("snow vertices")),
		flake::test::information::string_conversion_adapter<flakelib::marching_cubes::vertex_count>(
			std::tr1::bind(
				&flakes::snow_cover_vertices_,
				this)))
{
}

awl::main::exit_code const
flake::volume::tests::flakes::run()
{
	return
		flake::test::base::run();
}

flake::volume::tests::flakes::~flakes()
{
}

void
flake::volume::tests::flakes::render(
	sge::renderer::context::object &_context)
{
	sge::renderer::state::scoped scoped_state(
		_context,
		sge::renderer::state::list
			(this->feature_active(test::json_identifier(FCPPT_TEXT("wireframe")))
			?
			 	sge::renderer::state::draw_mode::line
			:
			 	sge::renderer::state::draw_mode::fill));

	_context.clear(
		sge::renderer::clear::parameters()
			.back_buffer(
				sge::image::colors::grey())
			.depth_buffer(
				sge::renderer::clear::depth_buffer_value(
					1.0f)));

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("models"))))
		models_.render(
			_context);

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("snowcover"))))
	{
		flake::volume::snow_cover::scoped scoped_snow_cover(
			snow_cover_,
			_context);

		marching_cubes_manager_.render(
			_context);
	}

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("flakes"))))
		flakes_.render(
			_context);

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("arrows"))))
		arrows_manager_.render(
			_context);

	test::base::render(
		_context);
}

void
flake::volume::tests::flakes::update()
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

	if(this->current_multiplier().get())
	{
		// Order of operations: Advection, Force Application, Projection

		// Advection
		velocity_buffer_ =
			semilagrangian_advection_.update_float4(
				flakelib::volume::boundary_buffer_view(
					boundary_buffer_->value()),
				flakelib::volume::simulation::stam::velocity(
					velocity_buffer_->value()),
				velocity_buffer_->value(),
				delta);

		// Wind source
		wind_source_.update(
			velocity_buffer_->value());

		// Outflow boundaries
		outflow_boundaries_.update(
			velocity_buffer_->value());

		// Projection
		flakelib::volume::unique_float_buffer_lock divergence =
			divergence_.update(
				flakelib::volume::boundary_buffer_view(
					boundary_buffer_->value()),
				velocity_buffer_->value());

		flakelib::volume::unique_float_buffer_lock pressure =
			jacobi_.update(
				flakelib::volume::simulation::stam::initial_guess_buffer_view(
					initial_guess_buffer_->value()),
				flakelib::volume::boundary_buffer_view(
					boundary_buffer_->value()),
				flakelib::volume::simulation::stam::rhs_buffer_view(
					divergence->value()));

		subtract_pressure_gradient_.update(
			flakelib::volume::velocity_buffer_view(
				velocity_buffer_->value()),
			flakelib::volume::boundary_buffer_view(
				boundary_buffer_->value()),
			flakelib::volume::simulation::stam::pressure_buffer_view(
				pressure->value()));

		flakes_mover_.update(
			10.0f * delta,
			flakelib::volume::velocity_buffer_view(
				velocity_buffer_->value()),
			flakelib::volume::boundary_buffer_view(
				boundary_buffer_->value()));

		if(sge::timer::reset_when_expired(snow_cover_update_) && this->feature_active(test::json_identifier(FCPPT_TEXT("marchingcubes"))))
		{
			snow_cover_vertices_ =
				marching_cubes_.update(
					flakelib::marching_cubes::density_view(
						snow_density_buffer_->value()));
		}

		wind_strength_modulator_.update(
			raw_delta);
	}

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("arrows"))))
		velocity_arrows_.update(
			velocity_buffer_->value());
}
