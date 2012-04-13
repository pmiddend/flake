#include <flake/catch_statements.hpp>
#include <flake/save_l8_texture_to_file.hpp>
#include <flake/volume/tests/flakes.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/splatter/box/object.hpp>
#include <flakelib/splatter/pen/object_impl.hpp>
#include <flakelib/splatter/pen/volume.hpp>
#include <flakelib/volume/conversion/planar_size_from_volume_size.hpp>
#include <sge/camera/coordinate_system/identity.hpp>
#include <sge/camera/first_person/parameters.hpp>
#include <sge/image/colors.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/key_event.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/float.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/texture/capabilities_field.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/planar_parameters.hpp>
#include <sge/renderer/texture/planar_scoped_ptr.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/parameters.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <fcppt/signal/connection.hpp>
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
	flake::test_base(
		_function_context,
		sge::window::title(
			FCPPT_TEXT("Flakes test")),
		sge::systems::cursor_option_field(
			sge::systems::cursor_option::exclusive)),
	simulation_size_(
		sge::parse::json::find_and_convert_member<sge::opencl::memory_object::dim3>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("tests/volume/flakes/simulation-size")))),
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
					FCPPT_TEXT("tests/volume/flakes/near-plane")))),
		sge::renderer::projection::far(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/volume/flakes/far-plane")))),
		sge::renderer::projection::fov(
			fcppt::math::deg_to_rad(
				sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
					this->configuration(),
					sge::parse::json::string_to_path(
						FCPPT_TEXT("tests/volume/flakes/fov")))))),
	key_callback_connection_(
		this->keyboard().key_callback(
			std::)),
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
					FCPPT_TEXT("tests/volume/flakes/arrow-scale")))),
		flakelib::volume::conversion::grid_scale(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/volume/flakes/grid-scale")))),
		flakelib::volume::conversion::origin(
			sge::renderer::vector3::null()),
		simulation_size_),
	draw_arrows_(
		true),
	wind_source_(
		this->program_context(),
		flakelib::volume::simulation::stam::wind_strength(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/volume/flakes/wind-strength"))))),
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
					FCPPT_TEXT("tests/volume/flakes/jacobi-iterations"))))),
	subtract_pressure_gradient_(
		this->program_context()),
	boundary_buffer_(
		fcppt::make_unique_ptr<flakelib::volume::float_buffer_lock>(
			fcppt::ref(
				this->buffer_pool()),
			simulation_size_.get())),
	velocity_buffer_(
		fcppt::make_unique_ptr<flakelib::volume::float4_buffer_lock>(
			fcppt::ref(
				this->buffer_pool()),
			simulation_size_.get())),
	fill_buffer_(
		this->program_context()),
	flakes_(
		this->renderer(),
		camera_,
		this->opencl_system().context(),
		this->image_system(),
		flake::volume::flakes::count(
			sge::parse::json::find_and_convert_member<flake::volume::flakes::count::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/volume/flakes/flake-count")))),
		flake::volume::flakes::minimum_size(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/volume/flakes/flake-minimum-size")))),
		flake::volume::flakes::maximum_size(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/volume/flakes/flake-maximum-size")))),
		simulation_size_),
	models_(
		this->renderer(),
		this->image_system(),
		camera_,
		flake::volume::model::sun_direction(
			sge::parse::json::find_and_convert_member<sge::renderer::vector3>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/volume/flakes/sun-direction"))))),
	obstacles_(
		models_,
		sge::parse::json::find_and_convert_member<sge::parse::json::array>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("tests/volume/flakes/obstacles"))),
		flakelib::volume::boundary_buffer_view(
			boundary_buffer_->value()),
		splatter_),
	delta_timer_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			boost::chrono::seconds(1)))
{
	fill_buffer_.apply(
		flakelib::buffer::linear_view<cl_float>(
			boundary_buffer_->value().buffer()),
		static_cast<cl_float>(
			0.0f));

	fill_buffer_.apply(
		flakelib::buffer::linear_view<cl_float>(
			velocity_buffer_->value().buffer()),
		static_cast<cl_float>(
			0.0f));

	/*
	flakelib::splatter::box::size const splat_size(
		sge::parse::json::find_and_convert_member<sge::opencl::memory_object::dim3>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("tests/volume/flakes/splat-size"))));

	splatter_.splat_volume_float(
		density_buffer_->value(),
		flakelib::splatter::pen::volume(
			flakelib::splatter::box::object(
				flakelib::splatter::box::position(
					sge::opencl::memory_object::vector3(
						simulation_size_.w()/2 - splat_size.get().w()/2,
						simulation_size_.h()/2 - splat_size.get().h()/2,
						simulation_size_.d()/2 - splat_size.get().d()/2)),
				splat_size),
			flakelib::splatter::pen::is_round(
				true),
			flakelib::splatter::pen::is_smooth(
				true),
			flakelib::splatter::pen::draw_mode::add,
			flakelib::splatter::pen::blend_factor(
				1.0f)),
		static_cast<cl_float>(
			1.0f));

	sge::renderer::texture::planar_scoped_ptr temporary_texture(
		this->renderer().create_planar_texture(
			sge::renderer::texture::planar_parameters(
				flakelib::volume::conversion::planar_size_from_volume_size(
					simulation_size_),
				sge::image::color::format::l8,
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field(
					sge::renderer::resource_flags::readable),
				sge::renderer::texture::capabilities_field::null())));

	sge::opencl::memory_object::image::planar temporary_texture_image(
		this->opencl_system().context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		*temporary_texture);

	conversion_object_.float_view_to_flat_volume_texture(
		density_buffer_->value(),
		temporary_texture_image,
		flakelib::volume::conversion::scaling_factor(
			1.0f),
		flakelib::volume::conversion::constant_addition(
			0.0f));

	std::cout << "Saving to file...\n";
	flake::save_l8_texture_to_file(
		this->image_system(),
		*temporary_texture,
		"/tmp/slices.png");
		*/
}

awl::main::exit_code const
flake::volume::tests::flakes::run()
{
	return
		flake::test_base::run();
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

	models_.render();
	//flakes_.render();

	if(draw_arrows_)
		arrows_manager_.render();


	test_base::render();
}

void
flake::volume::tests::flakes::update()
{
	test_base::update();

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
	}


	velocity_arrows_.update(
		flakelib::volume::velocity_buffer_view(
			velocity_buffer_->value()));
}

namespace
{
fcppt::string const
bool_to_string(
	bool const b)
{
	return
		b
		?
			fcppt::string(FCPPT_TEXT("on"))
		:
			fcppt::string(FCPPT_TEXT("off"));
}
}

void
flake::volume::tests::flakes::key_callback(
	sge::input::keyboard::key_event const &e)
{
	switch(e.key_code())
	{
		case sge::input::keyboard::key_code::f1:
			draw_arrows_ = !draw_arrows_;
			this->post_notification(
				notifications::text(
					FCPPT_TEXT("Arrows ")+
					bool_to_string(
						draw_arrows_)));
			break;
		default: break;
	}
}
