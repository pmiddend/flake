#include <flake/catch_statements.hpp>
#include <flake/media_path_from_string.hpp>
#include <flake/test/information/string_conversion_adapter.hpp>
#include <flake/volume/tests/smoke.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/splatter/box/object.hpp>
#include <flakelib/splatter/pen/object.hpp>
#include <flakelib/volume/retrieve_filled_float_buffer.hpp>
#include <flakelib/volume/retrieve_zero_float4_buffer.hpp>
#include <flakelib/volume/voxelize/from_raw_file.hpp>
#include <sge/camera/coordinate_system/identity.hpp>
#include <sge/camera/first_person/parameters.hpp>
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
	flake::volume::tests::smoke s(
		_function_context);

	return
		s.run();
}
FLAKE_CATCH_STATEMENTS

flake::volume::tests::smoke::smoke(
	awl::main::function_context const &_function_context)
:
	flake::test::base(
		_function_context,
		sge::window::title(
			FCPPT_TEXT("smoke test")),
		flake::test::json_identifier(
			FCPPT_TEXT("smoke")),
		fcppt::assign::make_container<test::feature_sequence>
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("arrows")),
				test::optional_key_code(
					sge::input::keyboard::key_code::f1)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("models")),
				test::optional_key_code(
					sge::input::keyboard::key_code::f2)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("wireframe")),
				test::optional_key_code(
					sge::input::keyboard::key_code::f3)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("windsource")),
				test::optional_key_code(
					sge::input::keyboard::key_code::f5)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("temperaturevisual")),
				test::optional_key_code(
					sge::input::keyboard::key_code::f6)))
			(test::feature(
				test::json_identifier(
					FCPPT_TEXT("smoke")),
				test::optional_key_code(
					sge::input::keyboard::key_code::f4))),
		sge::systems::cursor_option_field(
			sge::systems::cursor_option::exclusive)),
	simulation_size_(
		sge::parse::json::find_and_convert_member<sge::opencl::memory_object::dim3>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("simulation-size")))),
	buissnesq_ambient_temperature_(
		sge::parse::json::find_and_convert_member<cl_float>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("buissnesq/ambient-temperature")))),
	splat_temperature_(
		sge::parse::json::find_and_convert_member<cl_float>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("buissnesq/splat-temperature")))),
	camera_(
		sge::camera::first_person::parameters(
			this->keyboard(),
			this->mouse(),
			sge::camera::first_person::is_active(
				true),
			sge::camera::first_person::movement_speed(
				sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
					this->configuration(),
					sge::parse::json::string_to_path(
						FCPPT_TEXT("camera-movement-speed")))),
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
	buissnesq_(
		this->program_context(),
		flakelib::volume::simulation::stam::buissnesq::density_strength(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("buissnesq/density-strength")))),
		flakelib::volume::simulation::stam::buissnesq::temperature_strength(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("buissnesq/temperature-strength")))),
		buissnesq_ambient_temperature_),
	vorticity_(
		this->program_context(),
		this->buffer_pool()),
	boundary_buffer_(
		flakelib::volume::retrieve_filled_float_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get(),
			0.0f)),
	smoke_density_buffer_(
		flakelib::volume::retrieve_filled_float_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get(),
			0.0f)),
	temperature_buffer_(
		flakelib::volume::retrieve_filled_float_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get(),
			buissnesq_ambient_temperature_.get())),
	velocity_buffer_(
		flakelib::volume::retrieve_zero_float4_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get())),
	models_(
		this->renderer(),
		this->image_system(),
		camera_,
		flake::volume::model::sun_direction(
			sge::parse::json::find_and_convert_member<sge::renderer::vector3>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("sun-direction"))))),
	model_(
		models_,
		model::identifier(
			sge::parse::json::find_and_convert_member<fcppt::string>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("model-name")))),
		model::position(
			model::position::value_type(
				0.0f,
				0.0f,
				0.0f))),
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
	raycaster_(
		this->renderer(),
		this->opencl_system().context(),
		camera_,
		this->image_system(),
		conversion_object_,
		simulation_size_,
		flake::volume::density_visualization::raycaster::step_size(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("raycast-step-size")))),
		flake::volume::density_visualization::raycaster::debug_output(
			false)),
	delta_timer_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			boost::chrono::seconds(1)))
{
	/*
	flakelib::volume::voxelize::from_raw_file(
		flake::media_path_from_string(
			FCPPT_TEXT("models/")+
			sge::parse::json::find_and_convert_member<fcppt::string>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("model-name")))+
			FCPPT_TEXT(".raw")),
		boundary_buffer_->value(),
		this->opencl_system().command_queue());
		*/
}

awl::main::exit_code const
flake::volume::tests::smoke::run()
{
	return
		flake::test::base::run();
}

flake::volume::tests::smoke::~smoke()
{
}

void
flake::volume::tests::smoke::render()
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

	/*
	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("models"))))
		models_.render();
		*/

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("smoke"))))
		raycaster_.render();

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("arrows"))))
		arrows_manager_.render();

	test::base::render();
}

void
flake::volume::tests::smoke::update()
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
		raw_delta);

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
		if(this->feature_active(test::json_identifier(FCPPT_TEXT("windsource"))))
			wind_source_.update(
				velocity_buffer_->value());

		flakelib::volume::unique_float4_buffer_lock vorticity_buffer(
			vorticity_.apply_vorticity(
				flakelib::volume::boundary_buffer_view(
					boundary_buffer_->value()),
				flakelib::volume::simulation::stam::velocity(
					velocity_buffer_->value())));

		velocity_buffer_ =
			vorticity_.apply_confinement(
				vorticity_buffer->value(),
				flakelib::volume::simulation::stam::velocity(
					velocity_buffer_->value()),
				delta,
				flakelib::volume::simulation::stam::vorticity_strength(
					sge::parse::json::find_and_convert_member<cl_float>(
						this->configuration(),
						sge::parse::json::string_to_path(
							FCPPT_TEXT("vorticity-strength")))));

		buissnesq_.update(
			flakelib::volume::simulation::stam::velocity(
				velocity_buffer_->value()),
			flakelib::volume::simulation::stam::buissnesq::density_view(
				smoke_density_buffer_->value()),
			flakelib::volume::simulation::stam::buissnesq::temperature_view(
				temperature_buffer_->value()),
			delta);

		// Outflow boundaries
		outflow_boundaries_.update(
			velocity_buffer_->value());

		// Projection
		flakelib::volume::unique_float_buffer_lock divergence =
			divergence_.update(
				flakelib::volume::boundary_buffer_view(
					boundary_buffer_->value()),
				velocity_buffer_->value());

		flakelib::volume::unique_float_buffer_lock initial_guess_buffer_lock(
			fcppt::make_unique_ptr<flakelib::volume::float_buffer_lock>(
				fcppt::ref(
					this->buffer_pool()),
				velocity_buffer_->value().size()));

		fill_buffer_.apply(
			flakelib::buffer::linear_view<cl_float>(
				initial_guess_buffer_lock->value().buffer()),
			static_cast<cl_float>(
				0));

		flakelib::volume::unique_float_buffer_lock pressure =
			jacobi_.update(
				flakelib::volume::simulation::stam::initial_guess_buffer_view(
					initial_guess_buffer_lock->value()),
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

		smoke_density_buffer_ =
			semilagrangian_advection_.update_float(
				flakelib::volume::boundary_buffer_view(
					boundary_buffer_->value()),
				flakelib::volume::simulation::stam::velocity(
					velocity_buffer_->value()),
				smoke_density_buffer_->value(),
				delta);

		temperature_buffer_ =
			semilagrangian_advection_.update_float(
				flakelib::volume::boundary_buffer_view(
					boundary_buffer_->value()),
				flakelib::volume::simulation::stam::velocity(
					velocity_buffer_->value()),
				temperature_buffer_->value(),
				delta);

		wind_strength_modulator_.update(
			raw_delta);
	}

	if(this->feature_active(test::json_identifier(FCPPT_TEXT("temperaturevisual"))))
	{
		/*
		raycaster_.update(
			boundary_buffer_->value(),
			flakelib::volume::conversion::scaling_factor(
				1.0f),
			flakelib::volume::conversion::constant_addition(
				0.0f));
				*/
		raycaster_.update(
			temperature_buffer_->value(),
			flakelib::volume::conversion::scaling_factor(
				1.0f),
			flakelib::volume::conversion::constant_addition(
				-buissnesq_ambient_temperature_.get()));
	}
	else
	{
		raycaster_.update(
			smoke_density_buffer_->value(),
			flakelib::volume::conversion::scaling_factor(
				1.0f),
			flakelib::volume::conversion::constant_addition(
				0.0f));
	}

	flakelib::splatter::box::object const splat_zone(
		flakelib::splatter::box::position(
			flakelib::splatter::box::position::value_type(
				30,
				5,
				30)),
		flakelib::splatter::box::size(
			flakelib::splatter::box::size::value_type(
				10,
				10,
				10)));

	splatter_.splat_volume_float(
		smoke_density_buffer_->value(),
		flakelib::splatter::pen::volume(
			splat_zone,
			flakelib::splatter::pen::is_round(
				true),
			flakelib::splatter::pen::is_smooth(
				true),
			flakelib::splatter::pen::draw_mode::mix,
			flakelib::splatter::pen::blend_factor(
				1.0f)),
		static_cast<cl_float>(
			1.0f));

	splatter_.splat_volume_float(
		temperature_buffer_->value(),
		flakelib::splatter::pen::volume(
			splat_zone,
			flakelib::splatter::pen::is_round(
				true),
			flakelib::splatter::pen::is_smooth(
				true),
			flakelib::splatter::pen::draw_mode::mix,
			flakelib::splatter::pen::blend_factor(
				1.0f)),
		splat_temperature_);

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("arrows"))))
		velocity_arrows_.update(
			velocity_buffer_->value());
}
