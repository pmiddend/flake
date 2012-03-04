#include <flake/catch_statements.hpp>
#include <flake/planar/tests/simple.hpp>
#include <flakelib/media_path.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/cl/planar_image_view_to_float_buffer.hpp>
#include <sge/font/system.hpp>
#include <sge/image/colors.hpp>
#include <sge/image/view/const_object.hpp>
#include <sge/image2d/file.hpp>
#include <sge/image2d/system.hpp>
#include <sge/image2d/view/const_elements_wrapper.hpp>
#include <sge/image2d/view/size.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/texture/create_planar_from_view.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/parameters.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/string.hpp>
#include <fcppt/text.hpp>
#include <fcppt/chrono/seconds.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/math/dim/arithmetic.hpp>
#include <fcppt/math/dim/structure_cast.hpp>


awl::main::exit_code const
custom_main(
	awl::main::function_context const &_function_context)
try
{
	flake::planar::tests::simple s(
		_function_context);

	return
		s.run();
}
FLAKE_CATCH_STATEMENTS

flake::planar::tests::simple::simple(
	awl::main::function_context const &_function_context)
:
	flake::test_base(
		_function_context,
		sge::window::title(
			FCPPT_TEXT("Simple 2D Stam simulation")),
		sge::systems::cursor_option_field::null()),
	fill_buffer_(
		this->program_context()),
	wind_source_(
		this->program_context(),
		flakelib::planar::simulation::stam::wind_strength(
			sge::parse::json::find_and_convert_member<cl_float>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/planar/simple/wind-strength"))))),
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
					FCPPT_TEXT("tests/planar/simple/jacobi-iterations"))))),
	subtract_pressure_gradient_(
		this->program_context()),
	boundary_image_file_(
		this->image_system().load(
			flakelib::media_path()
				/ FCPPT_TEXT("boundaries")
				/
				sge::parse::json::find_and_convert_member<fcppt::string>(
					this->configuration(),
					sge::parse::json::string_to_path(
						FCPPT_TEXT("tests/planar/simple/boundary"))))),
	boundary_buffer_(
		this->buffer_pool(),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
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
	monitor_parent_(
		this->renderer(),
		this->opencl_system().command_queue(),
		this->font_system().create_font(
			flakelib::media_path_from_string(
				FCPPT_TEXT("fonts/main.ttf")),
			sge::parse::json::find_and_convert_member<sge::font::size_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/planar/simple/monitor-font-size")))),
		monitor::font_color(
			sge::image::colors::black())),
	monitor_planar_converter_(
		this->program_context()),
	velocity_arrows_(
		monitor_parent_,
		monitor::name(
			FCPPT_TEXT("velocity")),
		monitor::grid_dimensions(
			fcppt::math::dim::structure_cast<monitor::grid_dimensions::value_type>(
				sge::image2d::view::size(
					boundary_image_file_->view()))),
		monitor::arrow_scale(
			sge::parse::json::find_and_convert_member<monitor::arrow_scale::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/planar/simple/velocity-arrow-scale")))),
		monitor::grid_scale(
			sge::parse::json::find_and_convert_member<monitor::grid_scale::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/planar/simple/velocity-grid-scale")))),
		sge::renderer::texture::create_planar_from_view(
			this->renderer(),
			boundary_image_file_->view(),
			sge::renderer::texture::mipmap::off(),
			sge::renderer::resource_flags_field(
				sge::renderer::resource_flags::none))),
	smoke_density_texture_(
		monitor_parent_,
		monitor::name(
			FCPPT_TEXT("smoke density")),
		monitor::grid_dimensions(
			sge::parse::json::find_and_convert_member<monitor::grid_dimensions::value_type::value_type>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/planar/simple/texture-grid-scale"))) *
			fcppt::math::dim::structure_cast<monitor::grid_dimensions::value_type>(
				sge::image2d::view::size(
					boundary_image_file_->view()))),
		monitor::texture_size(
			fcppt::math::dim::structure_cast<monitor::dim>(
				sge::image2d::view::size(
					boundary_image_file_->view()))),
		monitor::scaling_factor(
			1.0f)),
	rucksack_viewport_adaptor_(
		this->viewport_manager(),
		this->renderer()),
	rucksack_enumeration_(
		rucksack::padding(
			sge::parse::json::find_and_convert_member<rucksack::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/planar/simple/master-and-slave-padding")))),
		rucksack::aspect(
			1,
			1)),
	delta_timer_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			fcppt::chrono::seconds(1))),
	clock_multiplier_(
		sge::parse::json::find_and_convert_member<float>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("tests/planar/simple/clock-multiplier"))))
{
	rucksack_viewport_adaptor_.child(
		rucksack_enumeration_);

	rucksack_enumeration_.push_back_child(
		velocity_arrows_.widget());

	rucksack_enumeration_.push_back_child(
		smoke_density_texture_.widget());

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
}

awl::main::exit_code const
flake::planar::tests::simple::run()
{
	return
		flake::test_base::run();
}

flake::planar::tests::simple::~simple()
{
}

void
flake::planar::tests::simple::render()
{
	sge::renderer::state::scoped scoped_state(
		this->renderer(),
		sge::renderer::state::list
			(sge::renderer::state::color::back_buffer_clear_color = sge::image::colors::grey()));

	this->renderer().clear(
		sge::renderer::clear_flags_field(
			sge::renderer::clear_flags::back_buffer));

	monitor_parent_.render(
		monitor::optional_projection());
}

void
flake::planar::tests::simple::update()
{
	monitor_parent_.update();

	flakelib::duration const delta =
		clock_multiplier_ *
		sge::timer::elapsed_and_reset<flakelib::duration>(
			delta_timer_);

	outflow_boundaries_.update(
		velocity_buffer_->value());

	wind_source_.update(
		velocity_buffer_->value());

	velocity_buffer_ =
		semilagrangian_advection_.update_planar(
			flakelib::planar::boundary_buffer_view(
				boundary_buffer_.value()),
			flakelib::planar::simulation::stam::velocity(
				velocity_buffer_->value()),
			velocity_buffer_->value(),
			delta);

	flakelib::planar::unique_float_buffer_lock divergence =
		divergence_.update(
			flakelib::planar::boundary_buffer_view(
				boundary_buffer_.value()),
			velocity_buffer_->value());

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

	subtract_pressure_gradient_.update(
		flakelib::planar::velocity_buffer_view(
			velocity_buffer_->value()),
		flakelib::planar::boundary_buffer_view(
			boundary_buffer_.value()),
		flakelib::planar::simulation::stam::pressure_buffer_view(
			pressure->value()));

	monitor_planar_converter_.to_arrow_vb(
		velocity_buffer_->value(),
		velocity_arrows_.cl_buffer(),
		velocity_arrows_.grid_scale(),
		velocity_arrows_.arrow_scale());

	monitor_planar_converter_.scalar_to_texture(
		smoke_density_buffer_->value(),
		smoke_density_texture_.cl_texture(),
		monitor::scaling_factor(
			1.0f));
}
