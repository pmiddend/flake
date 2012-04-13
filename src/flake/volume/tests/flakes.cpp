#include <flake/catch_statements.hpp>
#include <sge/timer/reset_when_expired.hpp>
#include <flake/volume/tests/flakes.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/volume/retrieve_zero_float4_buffer.hpp>
#include <flakelib/volume/retrieve_zero_float_buffer.hpp>
#include <sge/camera/coordinate_system/identity.hpp>
#include <sge/camera/first_person/parameters.hpp>
#include <sge/image/colors.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/float.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
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
					FCPPT_TEXT("flakes")),
				test::optional_key_code(
					sge::input::keyboard::key_code::f2))),
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
					FCPPT_TEXT("wind-strength"))))),
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
		flakelib::volume::retrieve_zero_float_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get())),
	snow_density_buffer_(
		flakelib::volume::retrieve_zero_float_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get())),
	activity_buffer_(
		flakelib::volume::retrieve_zero_float_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get())),
	velocity_buffer_(
		flakelib::volume::retrieve_zero_float4_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get())),
	flakes_(
		this->renderer(),
		camera_,
		this->opencl_system().context(),
		this->image_system(),
		flake::volume::flakes::count(
			sge::parse::json::find_and_convert_member<flake::volume::flakes::count::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("flake-count")))),
		flake::volume::flakes::minimum_size(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("flake-minimum-size")))),
		flake::volume::flakes::maximum_size(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("flake-maximum-size")))),
		simulation_size_),
	flakes_mover_(
		this->program_context(),
		flakes_.cl_positions(),
		volume::flakes::snow_density_view(
			snow_density_buffer_->value()),
		volume::flakes::collision_increment(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("collision-increment")))),
		volume::flakes::activity_view(
			activity_buffer_->value()),
		flakelib::marching_cubes::iso_level(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("iso-level"))))),
	models_(
		this->renderer(),
		this->image_system(),
		camera_,
		flake::volume::model::sun_direction(
			sge::parse::json::find_and_convert_member<sge::renderer::vector3>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("sun-direction"))))),
	obstacles_(
		models_,
		sge::parse::json::find_and_convert_member<sge::parse::json::array>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("obstacles"))),
		flakelib::volume::boundary_buffer_view(
			boundary_buffer_->value()),
		splatter_),
	marching_cubes_(
		this->renderer(),
		camera_,
		this->program_context(),
		simulation_size_,
		flakelib::marching_cubes::iso_level(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("iso-level"))))),
	delta_timer_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			boost::chrono::seconds(1))),
	snow_cover_update_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			boost::chrono::seconds(1)))
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
flake::volume::tests::flakes::render()
{
	sge::renderer::state::scoped scoped_state(
		this->renderer(),
		sge::renderer::state::list
			(sge::renderer::state::color::back_buffer_clear_color = sge::image::colors::black())
			(sge::renderer::state::float_::depth_buffer_clear_val = 1.0f));

	this->renderer().clear(
		sge::renderer::clear_flags_field(
			sge::renderer::clear_flags::back_buffer) | sge::renderer::clear_flags::depth_buffer);

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("models"))))
		models_.render();

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("flakes"))))
		flakes_.render();

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("arrows"))))
		arrows_manager_.render();

	if(
		this->feature_active(
			test::json_identifier(
				FCPPT_TEXT("snowcover"))))
		marching_cubes_.render();


	test::base::render();
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

		flakes_mover_.update(
			10.0f * delta,
			flakelib::volume::velocity_buffer_view(
				velocity_buffer_->value()),
			flakelib::volume::boundary_buffer_view(
				boundary_buffer_->value()));

		if(sge::timer::reset_when_expired(snow_cover_update_))
		{
			std::cout << "Updating snow cover\n";
			marching_cubes_.update(
				snow_density_buffer_->value());
		}
	}


	velocity_arrows_.update(
		flakelib::volume::velocity_buffer_view(
			velocity_buffer_->value()));
}
