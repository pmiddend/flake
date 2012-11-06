#include <flake/catch_statements.hpp>
#include <flake/media_path.hpp>
#include <flake/media_path_from_string.hpp>
#include <flake/tests/vorticity.hpp>
#include <flakelib/media_path.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/cl/planar_image_view_to_float_buffer.hpp>
#include <flakelib/splatter/pen/object_impl.hpp>
#include <flakelib/splatter/rectangle/object.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/camera/ortho_freelook/parameters.hpp>
#include <sge/image/colors.hpp>
#include <sge/image/view/const_object.hpp>
#include <sge/image2d/file.hpp>
#include <sge/image2d/system.hpp>
#include <sge/image2d/view/const_elements_wrapper.hpp>
#include <sge/image2d/view/size.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/clear/parameters.hpp>
#include <sge/renderer/context/ffp.hpp>
#include <sge/renderer/device/ffp.hpp>
#include <sge/renderer/texture/create_planar_from_view.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/parameters.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/string.hpp>
#include <fcppt/text.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/box/structure_cast.hpp>
#include <fcppt/math/dim/arithmetic.hpp>
#include <fcppt/math/dim/structure_cast.hpp>


awl::main::exit_code const
custom_main(
	awl::main::function_context const &);

awl::main::exit_code const
custom_main(
	awl::main::function_context const &_function_context)
try
{
	flake::tests::vorticity s(
		_function_context);

	return
		s.run();
}
FLAKE_CATCH_STATEMENTS

flake::tests::vorticity::vorticity(
	awl::main::function_context const &_function_context)
:
	flake::test::base(
		_function_context,
		sge::window::title(
			FCPPT_TEXT("vorticity 2D Stam simulation")),
		flake::test::json_identifier(
			FCPPT_TEXT("planar-vorticity")),
		flake::test::feature_sequence(),
		sge::systems::cursor_option_field::null()),
	fill_buffer_(
		this->program_context()),
	mix_buffers_(
		this->program_context()),
	splatter_(
		this->program_context()),
	wind_source_(
		this->program_context(),
		flakelib::planar::simulation::stam::wind_strength(
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
		flakelib::planar::simulation::stam::iterations(
			sge::parse::json::find_and_convert_member<flakelib::planar::simulation::stam::iterations::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("jacobi-iterations"))))),
	subtract_pressure_gradient_(
		this->program_context()),
	vorticity_(
		this->program_context(),
		this->buffer_pool()),
	boundary_image_file_(
		this->image_system().load(
			flake::media_path()
				/ FCPPT_TEXT("boundaries")
				/
				sge::parse::json::find_and_convert_member<fcppt::string>(
					this->configuration(),
					sge::parse::json::string_to_path(
						FCPPT_TEXT("boundary"))))),
	boundary_buffer_(
		this->buffer_pool(),
		fcppt::math::dim::structure_cast<sge::opencl::dim2>(
			sge::image2d::view::size(
				boundary_image_file_->view()))),
	velocity_buffer_(
		fcppt::make_unique_ptr<flakelib::planar::float2_buffer_lock>(
			fcppt::ref(
				this->buffer_pool()),
			boundary_buffer_.value().size())),
	smoke_density_buffer_(
		fcppt::make_unique_ptr<flakelib::planar::float_buffer_lock>(
			fcppt::ref(
				this->buffer_pool()),
			boundary_buffer_.value().size())),
	smoke_density_source_buffer_(
		fcppt::make_unique_ptr<flakelib::planar::float_buffer_lock>(
			fcppt::ref(
				this->buffer_pool()),
			boundary_buffer_.value().size())),
	boundary_texture_(
		sge::renderer::texture::create_planar_from_view(
			this->renderer(),
			boundary_image_file_->view(),
			sge::renderer::texture::mipmap::off(),
			sge::renderer::resource_flags_field(
				sge::renderer::resource_flags_field::null()),
			sge::renderer::texture::emulate_srgb::no)),
	monitor_parent_(
		this->renderer(),
		this->shader_context(),
		this->opencl_system().command_queue(),
		this->font_system(),
		sge::parse::json::find_and_convert_member<sge::font::ttf_size>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("monitor-font-size"))),
		flake::planar::monitor::font_color(
			sge::image::colors::black())),
	planar_converter_(
		this->program_context()),
	velocity_arrows_(
		monitor_parent_,
		flake::planar::monitor::name(
			FCPPT_TEXT("velocity")),
		flake::planar::monitor::grid_dimensions(
			fcppt::math::dim::structure_cast<flake::planar::monitor::grid_dimensions::value_type>(
				sge::image2d::view::size(
					boundary_image_file_->view()))),
		flake::planar::monitor::arrow_scale(
			sge::parse::json::find_and_convert_member<flake::planar::monitor::arrow_scale::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("velocity-arrow-scale")))),
		flake::planar::monitor::grid_scale(
			sge::parse::json::find_and_convert_member<flake::planar::monitor::grid_scale::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("velocity-grid-scale")))),
		flake::planar::monitor::optional_background_texture(
			*boundary_texture_)),
	smoke_density_texture_(
		monitor_parent_,
		flake::planar::monitor::name(
			FCPPT_TEXT("smoke density")),
		flake::planar::monitor::grid_dimensions(
			sge::parse::json::find_and_convert_member<flake::planar::monitor::grid_dimensions::value_type::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("texture-grid-scale"))) *
			fcppt::math::dim::structure_cast<flake::planar::monitor::grid_dimensions::value_type>(
				sge::image2d::view::size(
					boundary_image_file_->view()))),
		flake::planar::monitor::texture_size(
			fcppt::math::dim::structure_cast<flake::planar::monitor::dim>(
				sge::image2d::view::size(
					boundary_image_file_->view()))),
		flake::planar::monitor::scaling_factor(
			1.0f)),
	vorticity_texture_(
		monitor_parent_,
		flake::planar::monitor::name(
			FCPPT_TEXT("vorticity")),
		flake::planar::monitor::grid_dimensions(
			sge::parse::json::find_and_convert_member<flake::planar::monitor::grid_dimensions::value_type::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("texture-grid-scale"))) *
			fcppt::math::dim::structure_cast<flake::planar::monitor::grid_dimensions::value_type>(
				sge::image2d::view::size(
					boundary_image_file_->view()))),
		flake::planar::monitor::texture_size(
			fcppt::math::dim::structure_cast<flake::planar::monitor::dim>(
				sge::image2d::view::size(
					boundary_image_file_->view()))),
		flake::planar::monitor::scaling_factor(
			1.0f)),
	divergence_texture_(
		monitor_parent_,
		flake::planar::monitor::name(
			FCPPT_TEXT("divergence")),
		flake::planar::monitor::grid_dimensions(
			sge::parse::json::find_and_convert_member<flake::planar::monitor::grid_dimensions::value_type::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("texture-grid-scale"))) *
			fcppt::math::dim::structure_cast<flake::planar::monitor::grid_dimensions::value_type>(
				sge::image2d::view::size(
					boundary_image_file_->view()))),
		flake::planar::monitor::texture_size(
			fcppt::math::dim::structure_cast<flake::planar::monitor::dim>(
				sge::image2d::view::size(
					boundary_image_file_->view()))),
		flake::planar::monitor::scaling_factor(
			1.0f)),
	pressure_texture_(
		monitor_parent_,
		flake::planar::monitor::name(
			FCPPT_TEXT("pressure")),
		flake::planar::monitor::grid_dimensions(
			sge::parse::json::find_and_convert_member<flake::planar::monitor::grid_dimensions::value_type::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("texture-grid-scale"))) *
			fcppt::math::dim::structure_cast<flake::planar::monitor::grid_dimensions::value_type>(
				sge::image2d::view::size(
					boundary_image_file_->view()))),
		flake::planar::monitor::texture_size(
			fcppt::math::dim::structure_cast<flake::planar::monitor::dim>(
				sge::image2d::view::size(
					boundary_image_file_->view()))),
		flake::planar::monitor::scaling_factor(
			1.0f)),
	vorticity_gradient_arrows_(
		monitor_parent_,
		flake::planar::monitor::name(
			FCPPT_TEXT("vorticity gradient")),
		flake::planar::monitor::grid_dimensions(
			fcppt::math::dim::structure_cast<flake::planar::monitor::grid_dimensions::value_type>(
				sge::image2d::view::size(
					boundary_image_file_->view()))),
		flake::planar::monitor::arrow_scale(
			sge::parse::json::find_and_convert_member<flake::planar::monitor::arrow_scale::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("vorticity-gradient-arrow-scale")))),
		flake::planar::monitor::grid_scale(
			sge::parse::json::find_and_convert_member<flake::planar::monitor::grid_scale::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("vorticity-gradient-grid-scale")))),
		flake::planar::monitor::optional_background_texture(
			*boundary_texture_)),
	rucksack_viewport_adaptor_(
		this->viewport_manager(),
		this->renderer()),
	rucksack_enumeration_(
		sge::rucksack::padding(
			sge::parse::json::find_and_convert_member<sge::rucksack::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("master-and-slave-padding")))),
		sge::rucksack::aspect(
			1,
			1)),
	freelook_camera_(
		sge::camera::ortho_freelook::parameters(
			this->mouse(),
			this->keyboard(),
			sge::renderer::projection::near(
				0.0f),
			sge::renderer::projection::far(
				10.0f),
			sge::camera::is_active(
				true))),
	projection_rectangle_from_viewport_(
		freelook_camera_,
		this->viewport_manager()),
	cursor_splatter_(
		smoke_density_texture_,
		splatter_,
		this->renderer(),
		freelook_camera_,
		this->cursor(),
		sge::parse::json::find_and_convert_member<cl_float>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("smoke-density-splat-per-second"))),
		flakelib::splatter::pen::planar(
			flakelib::splatter::rectangle::object(
				flakelib::splatter::rectangle::position(
					flakelib::splatter::rectangle::position::value_type(
						0,
						0)),
				flakelib::splatter::rectangle::size(
					flakelib::splatter::rectangle::size::value_type(
						20,
						20))),
			flakelib::splatter::pen::is_round(
				true),
			flakelib::splatter::pen::is_smooth(
				true),
			flakelib::splatter::pen::draw_mode::add,
			flakelib::splatter::pen::blend_factor(
				0.5f))),
	delta_timer_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			boost::chrono::seconds(1)))
{
	rucksack_viewport_adaptor_.child(
		rucksack_enumeration_);

	rucksack_enumeration_.push_back_child(
		velocity_arrows_.widget());

	rucksack_enumeration_.push_back_child(
		smoke_density_texture_.widget());

	rucksack_enumeration_.push_back_child(
		vorticity_texture_.widget());

	rucksack_enumeration_.push_back_child(
		vorticity_gradient_arrows_.widget());

	rucksack_enumeration_.push_back_child(
		divergence_texture_.widget());

	rucksack_enumeration_.push_back_child(
		pressure_texture_.widget());

	flakelib::cl::planar_image_view_to_float_buffer(
		this->opencl_system().command_queue(),
		boundary_image_file_->view(),
		boundary_buffer_.value());

	fill_buffer_.apply(
		flakelib::buffer::linear_view<cl_float>(
			velocity_buffer_->value().buffer()),
		static_cast<cl_float>(
			0));

	fill_buffer_.apply(
		flakelib::buffer::linear_view<cl_float>(
			smoke_density_buffer_->value().buffer()),
		static_cast<cl_float>(
			0));

	fill_buffer_.apply(
		flakelib::buffer::linear_view<cl_float>(
			smoke_density_source_buffer_->value().buffer()),
		static_cast<cl_float>(
			0));

	cursor_splatter_.right_mouse_target(
		smoke_density_source_buffer_->value());
}

awl::main::exit_code const
flake::tests::vorticity::run()
{
	return
		flake::test::base::run();
}

flake::tests::vorticity::~vorticity()
{
}

void
flake::tests::vorticity::render(
	sge::renderer::context::ffp &_context)
{
	_context.clear(
		sge::renderer::clear::parameters()
			.back_buffer(
				sge::image::colors::grey()));

	monitor_parent_.render(
		_context,
		flake::planar::monitor::optional_projection(
			sge::camera::matrix_conversion::world_projection(
				freelook_camera_.coordinate_system(),
				freelook_camera_.projection_matrix())));
}

void
flake::tests::vorticity::update()
{
	test::base::update();

	monitor_parent_.update();

	flakelib::duration const
		raw_delta =
			sge::timer::elapsed_and_reset<flakelib::duration>(
				delta_timer_),
		delta =
			boost::rational_cast<flakelib::duration::rep>(
				this->current_multiplier().get()) *
			raw_delta;

	freelook_camera_.update(
		10.0f * raw_delta);

	if(this->current_multiplier().get())
	{
		// Order of operations: Advection, Force Application, Projection

		// Advection
		velocity_buffer_ =
			semilagrangian_advection_.update_float2(
				flakelib::planar::boundary_buffer_view(
					boundary_buffer_.value()),
				flakelib::planar::simulation::stam::velocity(
					velocity_buffer_->value()),
				velocity_buffer_->value(),
				delta);

		// Wind source
		wind_source_.update(
			velocity_buffer_->value());

		// Calculate vorticity
		flakelib::planar::unique_float_buffer_lock vorticity_buffer(
			vorticity_.apply_vorticity(
				flakelib::planar::boundary_buffer_view(
					boundary_buffer_.value()),
				flakelib::planar::simulation::stam::velocity(
					velocity_buffer_->value())));

		flakelib::planar::unique_float2_buffer_lock vorticity_gradient_buffer(
			vorticity_.confinement_data(
				vorticity_buffer->value(),
				delta,
				flakelib::planar::simulation::stam::vorticity_strength(
					sge::parse::json::find_and_convert_member<cl_float>(
						this->configuration(),
						sge::parse::json::string_to_path(
							FCPPT_TEXT("vorticity-strength"))))));

		// Apply vorticity
		velocity_buffer_ =
			vorticity_.apply_confinement(
				vorticity_buffer->value(),
				flakelib::planar::simulation::stam::velocity(
					velocity_buffer_->value()),
				delta,
				flakelib::planar::simulation::stam::vorticity_strength(
					sge::parse::json::find_and_convert_member<cl_float>(
						this->configuration(),
						sge::parse::json::string_to_path(
							FCPPT_TEXT("vorticity-strength")))));

		// Outflow boundaries
		outflow_boundaries_.update(
			velocity_buffer_->value());

		if(this->dump_this_frame())
			planar_converter_.arrows_to_matlab_files(
				velocity_buffer_->value(),
				flake::planar::conversion::x_coordinate_file(
					boost::filesystem::path(
						"/tmp/velocity_x.mat")),
				flake::planar::conversion::y_coordinate_file(
					boost::filesystem::path(
						"/tmp/velocity_y.mat")));

		planar_converter_.to_arrow_vb(
			vorticity_gradient_buffer->value(),
			vorticity_gradient_arrows_.cl_buffer(),
			flake::planar::conversion::grid_scale(
				vorticity_gradient_arrows_.grid_scale().get()),
			flake::planar::conversion::arrow_scale(
				vorticity_gradient_arrows_.arrow_scale().get()));

		if(this->dump_this_frame())
			planar_converter_.scalar_to_matlab_file(
				vorticity_buffer->value(),
				"/tmp/vorticity.mat");

		planar_converter_.scalar_to_texture(
			vorticity_buffer->value(),
			vorticity_texture_.cl_texture(),
			flake::planar::conversion::scaling_factor(
				0.50f),
			flake::planar::conversion::constant_addition(
				0.0f));

		// Projection
		flakelib::planar::unique_float_buffer_lock divergence =
			divergence_.update(
				flakelib::planar::boundary_buffer_view(
					boundary_buffer_.value()),
				velocity_buffer_->value());

		planar_converter_.scalar_to_texture(
			divergence->value(),
			divergence_texture_.cl_texture(),
			flake::planar::conversion::scaling_factor(
				0.50f),
			flake::planar::conversion::constant_addition(
				0.0f));

		flakelib::planar::unique_float_buffer_lock initial_guess_buffer_lock(
			fcppt::make_unique_ptr<flakelib::planar::float_buffer_lock>(
				fcppt::ref(
					this->buffer_pool()),
				velocity_buffer_->value().size()));

		fill_buffer_.apply(
			flakelib::buffer::linear_view<cl_float>(
				initial_guess_buffer_lock->value().buffer()),
			static_cast<cl_float>(
				0));

		flakelib::planar::unique_float_buffer_lock pressure =
			jacobi_.update(
				flakelib::planar::simulation::stam::initial_guess_buffer_view(
					initial_guess_buffer_lock->value()),
				flakelib::planar::boundary_buffer_view(
					boundary_buffer_.value()),
				flakelib::planar::simulation::stam::rhs_buffer_view(
					divergence->value()));

		if(this->dump_this_frame())
			planar_converter_.scalar_to_matlab_file(
				pressure->value(),
				"/tmp/pressure.mat");

		planar_converter_.scalar_to_texture(
			pressure->value(),
			pressure_texture_.cl_texture(),
			flake::planar::conversion::scaling_factor(
				0.10f),
			flake::planar::conversion::constant_addition(
				0.0f));

		subtract_pressure_gradient_.update(
			flakelib::planar::velocity_buffer_view(
				velocity_buffer_->value()),
			flakelib::planar::boundary_buffer_view(
				boundary_buffer_.value()),
			flakelib::planar::simulation::stam::pressure_buffer_view(
				pressure->value()));

		// "Other stuff"
		smoke_density_buffer_ =
			semilagrangian_advection_.update_float(
				flakelib::planar::boundary_buffer_view(
					boundary_buffer_.value()),
				flakelib::planar::simulation::stam::velocity(
					velocity_buffer_->value()),
				smoke_density_buffer_->value(),
				delta);

		// Left mouse splats directly
		cursor_splatter_.left_mouse_target(
			smoke_density_buffer_->value());

		cursor_splatter_.update(
			delta);

		// Right mouse splats to sources surface, which we now add
		mix_buffers_.add_from_to(
			flakelib::buffer::linear_view<cl_float>(
				smoke_density_source_buffer_->value().buffer()),
			flakelib::buffer::linear_view<cl_float>(
				smoke_density_buffer_->value().buffer()));
	}

	planar_converter_.to_arrow_vb(
		velocity_buffer_->value(),
		velocity_arrows_.cl_buffer(),
		flake::planar::conversion::grid_scale(
			velocity_arrows_.grid_scale().get()),
		flake::planar::conversion::arrow_scale(
			velocity_arrows_.arrow_scale().get()));

	planar_converter_.scalar_to_texture(
		smoke_density_buffer_->value(),
		smoke_density_texture_.cl_texture(),
		flake::planar::conversion::scaling_factor(
			1.0f),
		flake::planar::conversion::constant_addition(
			0.0f));


}
