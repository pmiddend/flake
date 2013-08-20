#include <flake/catch_statements.hpp>
#include <flake/media_path_from_string.hpp>
#include <flake/tests/density_visualization.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <sge/camera/coordinate_system/identity.hpp>
#include <sge/camera/first_person/parameters.hpp>
#include <sge/image/color/predef.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/clear/parameters.hpp>
#include <sge/renderer/context/ffp.hpp>
#include <sge/renderer/device/ffp.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/parameters.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <chrono>
#include <fcppt/config/external_end.hpp>


awl::main::exit_code const
custom_main(
	awl::main::function_context const &);

awl::main::exit_code const
custom_main(
	awl::main::function_context const &_function_context)
try
{
	flake::tests::density_visualization s(
		_function_context);

	return
		s.run();
}
FLAKE_CATCH_STATEMENTS

flake::tests::density_visualization::density_visualization(
	awl::main::function_context const &_function_context)
:
	flake::test::base(
		_function_context,
		sge::window::title(
			FCPPT_TEXT("flake: density visualization test")),
		flake::test::json_identifier(
			FCPPT_TEXT("density-visualization")),
		test::feature_sequence(),
		/*
		fcppt::assign::make_container<test::feature_sequence>(
			 flake::test::feature(
			 	flake::test::json_identifier(
					FCPPT_TEXT("wireframe")),
				flake::test::optional_key_code(
					sge::input::keyboard::key_code::f2)))*/
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
			sge::camera::is_active(
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
	conversion_(
		this->program_context()),
	/*
	splatter_(
		this->program_context()),
		*/
	density_buffer_(
		conversion_.binvox_file_to_buffer(
			this->buffer_pool(),
			flake::media_path_from_string(
				FCPPT_TEXT("voxelized_models/")+
				sge::parse::json::find_and_convert_member<fcppt::string>(
					this->configuration(),
					sge::parse::json::string_to_path(
						FCPPT_TEXT("voxel-file/name")))),
			flakelib::volume::conversion::optional_height())
		/*
		flakelib::volume::retrieve_filled_float_buffer(
			this->buffer_pool(),
			fill_buffer_,
			simulation_size_.get(),
			0.0f)*/),
	raycaster_(
		this->renderer(),
		this->shader_context(),
		this->opencl_system().context(),
		camera_,
		this->image_system(),
		conversion_,
		simulation_size_,
		flake::volume::density_visualization::raycaster::step_size(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("raycast-step-size")))),
		flake::volume::density_visualization::raycaster::debug_output(
			false)),
/*
	models_(
		this->renderer(),
		this->shader_context(),
		this->image_system(),
		camera_,
		flake::volume::model::sun_direction(
			sge::parse::json::find_and_convert_member<sge::renderer::vector3>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("sun-direction")))),
		flake::volume::model::fog_color(
			sge::parse::json::find_and_convert_member<sge::renderer::vector3>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("fog-color")))),
		flake::volume::model::fog_density(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				this->configuration(),
				sge::parse::json::string_to_path(
					FCPPT_TEXT("fog-density"))))),
	avz_(
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
*/
	delta_timer_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			std::chrono::seconds(1)))
{
	/*
	splatter_.splat_volume_float(
		density_buffer_->value(),
		flakelib::splatter::pen::volume(
			flakelib::splatter::box::object(
				flakelib::splatter::box::position(
					flakelib::splatter::box::position::value_type(
						1,
						1,
						1)),
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
			*/
}

awl::main::exit_code const
flake::tests::density_visualization::run()
{
	return
		flake::test::base::run();
}

flake::tests::density_visualization::~density_visualization()
{
}

void
flake::tests::density_visualization::render(
	sge::renderer::context::ffp &_context)
{
	_context.clear(
		sge::renderer::clear::parameters()
			.depth_buffer(
				sge::renderer::clear::depth_buffer_value(
					1.0f))
			.back_buffer(
				sge::image::color::predef::black()));

	raycaster_.render(
		_context);

	/*
	models_.render(
		_context);
		*/
}

void
flake::tests::density_visualization::update()
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

	raycaster_.update(
		density_buffer_->value(),
		flakelib::volume::conversion::scaling_factor(
			1.0f),
		flakelib::volume::conversion::constant_addition(
			0.0f));
}
