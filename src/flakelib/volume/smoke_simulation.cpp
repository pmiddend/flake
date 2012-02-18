#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/utility/object.hpp>
#include <flakelib/volume/smoke_simulation.hpp>
#include <flakelib/volume/boundary/object.hpp>
#include <flakelib/volume/boundary/json/to_obstacle_sequence.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <flakelib/volume/density/advector.hpp>
#include <flakelib/volume/density/strength_modulator.hpp>
#include <flakelib/volume/density/visual.hpp>
#include <flakelib/volume/flakes/object.hpp>
#include <flakelib/volume/laplace_solver/jacobi.hpp>
#include <flakelib/volume/simulation/stam/object.hpp>
#include <flakelib/volume/visualization/arrows.hpp>
#include <flakelib/volume/visualization/arrows_manager.hpp>
#include <flakelib/volume/visualization/ground.hpp>
#include <flakelib/volume/visualization/shape_manager.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/dim3.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/object.hpp>
#include <sge/shader/scoped.hpp>
#include <fcppt/cref.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/math/dim/basic_impl.hpp>


flakelib::volume::smoke_simulation::smoke_simulation(
	sge::opencl::command_queue::object &_command_queue,
	sge::renderer::device &_renderer,
	sge::camera::base &_camera,
	sge::image2d::system &_image_system,
	flakelib::build_options const &_build_options,
	sge::parse::json::object const &_json_config)
:
	utility_(
		fcppt::make_unique_ptr<flakelib::utility::object>(
			fcppt::ref(
				_command_queue),
			_build_options)),
	buffer_pool_(
		fcppt::make_unique_ptr<buffer_pool::object>(
			fcppt::ref(
				_command_queue.context()))),
	boundary_(
		fcppt::make_unique_ptr<boundary::object>(
			fcppt::ref(
				_command_queue),
			fcppt::ref(
				*utility_),
			fcppt::ref(
				*buffer_pool_),
			_build_options,
			volume::grid_size(
				sge::parse::json::find_and_convert_member<sge::opencl::memory_object::dim3>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("grid-size")))),
			boundary::json::to_obstacle_sequence(
				sge::parse::json::find_and_convert_member<sge::parse::json::array>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("obstacles")))),
			boundary::pave_ground(
				false))),
	laplace_solver_(
		fcppt::make_unique_ptr<laplace_solver::jacobi>(
			fcppt::ref(
				*buffer_pool_),
			fcppt::ref(
				_command_queue),
			_build_options,
			flakelib::volume::laplace_solver::boundary(
				boundary_->get()),
			flakelib::volume::laplace_solver::iterations(
				sge::parse::json::find_and_convert_member<unsigned>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("jacobi-iterations")))))),
	simulation_(
		fcppt::make_unique_ptr<simulation::stam::object>(
			fcppt::ref(
				_command_queue),
			boundary::view(
				boundary_->get()),
			simulation::stam::external_force_magnitude(
				sge::parse::json::find_and_convert_member<cl_float>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("external-force-magnitude")))),
			_build_options,
			fcppt::ref(
				*buffer_pool_),
			fcppt::ref(
				*utility_),
			fcppt::ref(
				*laplace_solver_),
			simulation::stam::use_maccormack(
				true))),
	shape_manager_(
		fcppt::make_unique_ptr<visualization::shape_manager>(
			fcppt::ref(
				_renderer),
			fcppt::ref(
				_camera),
			fcppt::ref(
				_image_system),
			boundary::json::to_obstacle_sequence(
				sge::parse::json::find_and_convert_member<sge::parse::json::array>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("obstacles")))),
			visualization::movement_hack(
				false),
			visualization::scaling_hack(
				true),
			visualization::sun_direction(
				sge::parse::json::find_and_convert_member<sge::renderer::vector3>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("sun-direction")))))),
	conversion_(
		fcppt::make_unique_ptr<conversion::object>(
			fcppt::ref(
				_command_queue),
			_build_options)),
	arrows_manager_(
		fcppt::make_unique_ptr<visualization::arrows_manager>(
			fcppt::ref(
				_renderer))),
	arrows_(
		fcppt::make_unique_ptr<visualization::arrows>(
			fcppt::ref(
				_command_queue.context()),
			fcppt::ref(
				*arrows_manager_),
			fcppt::ref(
				*conversion_),
			visualization::grid_size(
				boundary_->get().size()),
			conversion::arrow_scale(
				sge::parse::json::find_and_convert_member<cl_float>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("arrow-scale")))))),
	density_advector_(
		fcppt::make_unique_ptr<density::advector>(
			fcppt::ref(
				_command_queue),
			boundary::view(
				boundary_->get()),
			_build_options,
			fcppt::ref(
				*buffer_pool_),
			fcppt::ref(
				*utility_))),
	density_visual_(
		fcppt::make_unique_ptr<density::visual>(
			fcppt::ref(
				_renderer),
			fcppt::ref(
				_image_system),
			fcppt::ref(
				_command_queue),
			_build_options,
			volume::grid_size(
				boundary_->get().size()),
			boundary::view(
				boundary_->get()))),
	strength_modulator_(
		fcppt::make_unique_ptr<density::strength_modulator>(
			fcppt::ref(
				*density_advector_),
			density::strength_minimum(
				sge::parse::json::find_and_convert_member<cl_float>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("density-strength-minimum")))),
			density::strength_maximum(
				sge::parse::json::find_and_convert_member<cl_float>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("density-strength-maximum")))),
			density::change_frequency(
				sge::parse::json::find_and_convert_member<cl_float>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("density-change-frequency")))))),
	draw_arrows_(
		false)
{
}

void
flakelib::volume::smoke_simulation::update(
	flakelib::duration const &_duration)
{
	simulation_->update(
		_duration);

	if(draw_arrows_)
		arrows_->convert(
			simulation_->velocity());

	strength_modulator_->update(
		_duration);

	density_advector_->update(
		simulation_->velocity(),
		_duration);

	density_visual_->update(
		density_advector_->get());
}

void
flakelib::volume::smoke_simulation::render(
	sge::renderer::vector3 const &_camera_position,
	sge::renderer::matrix4 const &_mvp)
{
	shape_manager_->render();

	if(draw_arrows_)
	{
		sge::shader::scoped scoped_shader(
			arrows_manager_->shader(),
			sge::shader::activate_everything());

		arrows_manager_->camera_position(
			_camera_position);

		arrows_manager_->shader().update_uniform(
			"mvp",
			sge::shader::matrix(
				_mvp,
				sge::shader::matrix_flags::projection));

		arrows_->render();
	}

	density_visual_->render(
		_camera_position,
		_mvp);
}

void
flakelib::volume::smoke_simulation::toggle_arrows()
{
	draw_arrows_ = !draw_arrows_;
}

void
flakelib::volume::smoke_simulation::external_force_magnitude(
	cl_float const _external_force)
{
	simulation_->external_force_magnitude(
		_external_force);
}

flakelib::volume::smoke_simulation::~smoke_simulation()
{
}
