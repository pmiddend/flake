#include <flake/catch_statements.hpp>
#include <flake/media_path_from_string.hpp>
#include <flake/test/information/string_conversion_adapter.hpp>
#include <flake/tests/flakes.hpp>
#include <flake/volume/snow_cover/parallel_update.hpp>
#include <flake/volume/snow_cover/scoped.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/marching_cubes/vf/format.hpp>
#include <flakelib/volume/retrieve_filled_float_buffer.hpp>
#include <flakelib/volume/retrieve_zero_float4_buffer.hpp>
#include <sge/camera/coordinate_system/identity.hpp>
#include <sge/camera/first_person/parameters.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/image/color/predef.hpp>
#include <sge/input/keyboard/optional_key_code.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/clear/parameters.hpp>
#include <sge/renderer/context/ffp.hpp>
#include <sge/renderer/device/ffp.hpp>
#include <sge/renderer/state/core/rasterizer/object.hpp>
#include <sge/renderer/state/core/rasterizer/object_unique_ptr.hpp>
#include <sge/renderer/state/core/rasterizer/parameters.hpp>
#include <sge/renderer/state/core/rasterizer/scoped.hpp>
#include <sge/renderer/target/onscreen.hpp>
#include <sge/renderer/texture/create_planar_from_path.hpp>
#include <sge/renderer/texture/emulate_srgb.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/mipmap/all_levels.hpp>
#include <sge/scenic/render_context/base.hpp>
#include <sge/scenic/scene/from_blender_file.hpp>
#include <sge/scenic/scene/prototype.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/parameters.hpp>
#include <sge/timer/reset_when_expired.hpp>
#include <fcppt/insert_to_fcppt_string.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/strong_typedef_output.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/cast/size_fun.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <memory>
#include <fcppt/config/external_end.hpp>


awl::main::exit_code const
custom_main(
	awl::main::function_context const &);

awl::main::exit_code const
custom_main(
	awl::main::function_context const &_function_context)
try
{
	flake::tests::flakes s(
		_function_context);

	return
		s.run();
}
FLAKE_CATCH_STATEMENTS

flake::tests::flakes::flakes(
	awl::main::function_context const &_function_context)
:
	flake::test::base(
		_function_context,
		sge::window::title(
			FCPPT_TEXT("flake: Flakes test")),
		flake::test::json_identifier(
			FCPPT_TEXT("flakes")),
		fcppt::assign::make_container<test::feature_sequence>
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("arrows")),
				sge::input::keyboard::optional_key_code(
					sge::input::keyboard::key_code::f1)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("snowcover")),
				sge::input::keyboard::optional_key_code(
					sge::input::keyboard::key_code::f3)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("models")),
				sge::input::keyboard::optional_key_code(
					sge::input::keyboard::key_code::f4)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("wireframe")),
				sge::input::keyboard::optional_key_code(
					sge::input::keyboard::key_code::f5)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("marchingcubes")),
				sge::input::keyboard::optional_key_code(
					sge::input::keyboard::key_code::f6)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("flakes")),
				sge::input::keyboard::optional_key_code(
					sge::input::keyboard::key_code::f2))),
		sge::systems::cursor_option_field{
			sge::systems::cursor_option::exclusive}),
	simulation_size_(
		sge::parse::json::find_and_convert_member<sge::opencl::dim3>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("simulation-size")))),
	camera_manager_(
		sge::parse::json::find_and_convert_member<sge::parse::json::object>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("camera"))),
		this->keyboard(),
		this->mouse(),
		this->viewport_manager()),
	fill_buffer_(
		this->program_context()),
	splatter_(
		this->program_context()),
	conversion_object_(
		this->program_context()),
	arrows_manager_(
		this->renderer(),
		this->shader_context(),
		camera_manager_.camera()),
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
		conversion_object_.binvox_file_to_buffer(
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
	snow_density_buffer_(
		this->opencl_system().context(),
		sge::opencl::memory_object::flags_field{
			sge::opencl::memory_object::flags::read,
			sge::opencl::memory_object::flags::write,
			sge::opencl::memory_object::flags::alloc_host_ptr},
		sge::opencl::memory_object::byte_size(
			simulation_size_.get().content() * sizeof(cl_float))),
	snow_density_view_(
		flakelib::volume::float_view(
			snow_density_buffer_,
			fcppt::math::dim::structure_cast<
				sge::opencl::dim3,
				fcppt::cast::size_fun>(
				simulation_size_.get()))),
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
	skydome_(
		this->renderer(),
		this->shader_context(),
		this->image_system(),
		flake::skydome::texture_path(
			flake::media_path_from_string(
				FCPPT_TEXT("textures/sky.png"))),
		camera_manager_.camera(),
		flake::skydome::longitude(
			40u),
		flake::skydome::latitude(
			20u),
		flake::skydome::y_translation(
			-0.2f)),
	current_flake_count_(
		sge::parse::json::find_and_convert_member<flake::volume::flakes::count::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("flakes/start-count")))),
	flakes_(
		this->renderer(),
		this->shader_context(),
		camera_manager_.camera(),
		this->opencl_system().context(),
		this->image_system(),
		flake::volume::flakes::count(
			sge::parse::json::find_and_convert_member<flake::volume::flakes::count::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("flakes/maximum-count")))),
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
		snow_density_view_,
		flake::volume::flakes::collision_increment(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("collision-increment")))),
		flake::volume::flakes::activity_view(
			activity_buffer_->value()),
		flakes_.minimum_size(),
		flakes_.maximum_size(),
		flake::volume::flakes::gravity_magnitude(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("gravity-magnitude")))),
		flakelib::marching_cubes::iso_level(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("iso-level"))))),
	scene_manager_(
		this->renderer(),
		this->image_system(),
		sge::scenic::scene::prefer_cg_context(
			true)),
	scene_(
		scene_manager_,
		this->viewport_manager(),
		camera_manager_.camera(),
		sge::scenic::scene::from_blender_file(
			flake::media_path_from_string(
				FCPPT_TEXT("scenes/")+
				sge::parse::json::find_and_convert_member<fcppt::string>(
					this->configuration(),
					sge::parse::json::string_to_path(
						FCPPT_TEXT("scene-name")))+
				FCPPT_TEXT("/description.json")))),
	marching_cubes_manager_(
		this->renderer(),
		this->opencl_system().command_queue(),
		flakelib::marching_cubes::cpu::grid_size(
			fcppt::math::dim::structure_cast<
				sge::renderer::dim3,
				fcppt::cast::size_fun>(
				simulation_size_.get())),
		flakelib::marching_cubes::iso_level(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("iso-level")))),
		flakelib::volume::boundary_buffer_view(
			boundary_buffer_->value())),
	snow_cover_(
		camera_manager_.camera(),
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
					sge::renderer::resource_flags_field::null(),
					sge::renderer::texture::emulate_srgb::no))),
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
					sge::renderer::resource_flags_field::null(),
					sge::renderer::texture::emulate_srgb::no))),
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
	snow_cover_parallel_update_(
		sge::parse::json::find_and_convert_member<bool>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("update-snow-cover")))
		?
			fcppt::make_unique_ptr<flake::volume::snow_cover::parallel_update>(
				marching_cubes_manager_,
				this->opencl_system().command_queue(),
				snow_density_view_)
		:
			std::unique_ptr<flake::volume::snow_cover::parallel_update>()),
/*
		flake::volume::flakes::snow_density_view(
			flakelib::volume::float_view(
				activity_buffer_->value()))),
*/
	wind_strength_modulator_(
		std::bind(
			&flakelib::volume::simulation::stam::wind_source::wind_strength,
			&wind_source_,
			std::placeholders::_1),
		flakelib::value_modulator::mean(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("wind-strength-mean")))),
		flakelib::value_modulator::variance(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("wind-strength-variance")))),
		flakelib::value_modulator::frequency(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("wind-strength-frequency-seconds"))))),
	delta_timer_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			std::chrono::seconds(1))),
	snow_cover_update_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			std::chrono::milliseconds(1000)))/*,
	snow_cover_vertices_(
		0u),
	snow_cover_vertices_information_(
		this->information_manager(),
		test::information::identifier(
			FCPPT_TEXT("snow vertices")),
		flake::test::information::string_conversion_adapter<flakelib::marching_cubes::gpu::vertex_count>(
			std::bind(
				&flakes::snow_cover_vertices_,
				this)))*/
{
	fill_buffer_.apply(
		flakelib::buffer::linear_view<cl_float>(
			snow_density_buffer_),
		0.0f);
}

awl::main::exit_code const
flake::tests::flakes::run()
{
	return
		flake::test::base::run();
}

flake::tests::flakes::~flakes()
{
}

void
flake::tests::flakes::render(
	sge::renderer::context::ffp &_context)
{
	sge::renderer::state::core::rasterizer::object_unique_ptr const rasterizer_state(
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

	_context.clear(
		sge::renderer::clear::parameters()
			.back_buffer(
				sge::image::color::predef::grey())
			.depth_buffer(
				sge::renderer::clear::depth_buffer_value(
					1.0f)));

	{
		sge::renderer::state::core::rasterizer::scoped scoped_rasterizer_state(
			_context,
			*rasterizer_state);

		skydome_.render(
			_context);
	}

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("models"))))
	{
		sge::scenic::render_context::base_unique_ptr wrapped_context(
			scene_manager_.create_render_context(
				_context));

		sge::renderer::state::core::rasterizer::scoped scoped_rasterizer_state(
			_context,
			*rasterizer_state);

		scene_.render(
			*wrapped_context);
	}

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("snowcover"))))
	{
		flake::volume::snow_cover::scoped scoped_snow_cover(
			snow_cover_,
			_context);

		sge::renderer::state::core::rasterizer::scoped scoped_rasterizer_state(
			_context,
			*rasterizer_state);

		marching_cubes_manager_.render(
			_context);
	}

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("flakes"))))
		flakes_.render(
			_context,
			current_flake_count_);

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("arrows"))))
		arrows_manager_.render(
			_context);
}

void
flake::tests::flakes::update()
{
	test::base::update();

	flakelib::duration const
		raw_delta(
			0.25f *
			sge::timer::elapsed_and_reset<flakelib::duration>(
				delta_timer_)),
		delta(
			boost::rational_cast<flakelib::duration::rep>(
				this->current_multiplier().get()) *
			raw_delta);

	camera_manager_.update(
		40.0f * raw_delta);

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
				boundary_buffer_->value()),
			current_flake_count_);

		if(
			snow_cover_parallel_update_ &&
			sge::timer::reset_when_expired(snow_cover_update_) &&
			this->feature_active(test::json_identifier(FCPPT_TEXT("marchingcubes"))))
			snow_cover_parallel_update_->restart_if_finished();
		/*
		{
			snow_cover_vertices_ =
				marching_cubes_.update(
					flakelib::marching_cubes::gpu::density_view(
						snow_density_buffer_->value()));
		}
		*/

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

void
flake::tests::flakes::key_down_callback(
	sge::input::keyboard::key_code const _key)
{
	switch(_key)
	{
	case sge::input::keyboard::key_code::period:
		wind_strength_modulator_.mean(
			wind_strength_modulator_.mean() +
			flakelib::value_modulator::mean(
				1.0f));
		this->post_notification(
			flake::notifications::text(
				FCPPT_TEXT("Wind speed mean: ")+
				fcppt::insert_to_fcppt_string(
					wind_strength_modulator_.mean().get())));
		break;
	case sge::input::keyboard::key_code::comma:
		wind_strength_modulator_.mean(
			wind_strength_modulator_.mean() -
			flakelib::value_modulator::mean(
				1.0f));
		this->post_notification(
			flake::notifications::text(
				FCPPT_TEXT("Wind speed mean: ")+
				fcppt::insert_to_fcppt_string(
					wind_strength_modulator_.mean().get())));
		break;
	case sge::input::keyboard::key_code::_1:
		current_flake_count_ =
			std::max(
				static_cast<flake::volume::flakes::count::value_type>(
					1),
				static_cast<flake::volume::flakes::count::value_type>(
					current_flake_count_.get() / 2u));

		this->post_notification(
			flake::notifications::text(
				FCPPT_TEXT("Flake count: ")+
				fcppt::insert_to_fcppt_string(
					current_flake_count_)));
		break;
	case sge::input::keyboard::key_code::_2:
		current_flake_count_ =
			std::min(
				flakes_.maximum_count().get(),
				static_cast<flake::volume::flakes::count::value_type>(
					current_flake_count_.get() * 2u));

		this->post_notification(
			flake::notifications::text(
				FCPPT_TEXT("Flake count: ")+
				fcppt::insert_to_fcppt_string(
					current_flake_count_)));
		break;
	default:
		break;
	}
}
