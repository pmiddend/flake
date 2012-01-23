#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/utility/object.hpp>
#include <flakelib/volume/flake_simulation.hpp>
#include <flakelib/volume/boundary/object.hpp>
#include <flakelib/volume/boundary/json/to_obstacle_sequence.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <flakelib/volume/density/advector.hpp>
#include <flakelib/volume/density/visual.hpp>
#include <flakelib/volume/flakes/object.hpp>
#include <flakelib/volume/laplace_solver/jacobi.hpp>
#include <flakelib/volume/simulation/stam/object.hpp>
#include <flakelib/volume/simulation/stam/wind_blower.hpp>
#include <flakelib/volume/visualization/arrows.hpp>
#include <flakelib/volume/visualization/arrows_manager.hpp>
#include <flakelib/volume/visualization/ground.hpp>
#include <flakelib/volume/visualization/shape_manager.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/camera/base.hpp>
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


flakelib::volume::flake_simulation::flake_simulation(
	sge::opencl::command_queue::object &_command_queue,
	sge::renderer::device &_renderer,
	sge::camera::base &_camera,
	sge::image2d::system &_image_system,
	flakelib::build_options const &_build_options,
	sge::parse::json::object const &_json_config)
:
	camera_(
		_camera),
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
				true))),
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
			simulation::stam::profiling_enabled(
				sge::parse::json::find_and_convert_member<bool>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("profiling-enabled")))),
			_build_options,
			fcppt::ref(
				*buffer_pool_),
			fcppt::ref(
				*utility_),
			fcppt::ref(
				*laplace_solver_),
			simulation::stam::use_maccormack(
				false))),
	shape_manager_(
		fcppt::make_unique_ptr<visualization::shape_manager>(
			fcppt::ref(
				_renderer),
			fcppt::ref(
				_image_system),
			boundary::json::to_obstacle_sequence(
				sge::parse::json::find_and_convert_member<sge::parse::json::array>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("obstacles")))),
			visualization::movement_hack(
				true),
			visualization::scaling_hack(
				false))),
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
	flakes_(
		fcppt::make_unique_ptr<flakes::object>(
			fcppt::ref(
				_renderer),
			fcppt::ref(
				_camera),
			fcppt::ref(
				_image_system),
			fcppt::ref(
				_command_queue),
			boundary::view(
				boundary_->get()),
			_build_options,
			flakes::particle_count(
				sge::parse::json::find_and_convert_member<flakes::particle_count::value_type>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("particle-count")))),
			flakes::particle_minimum_size(
				sge::parse::json::find_and_convert_member<cl_float>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("particle-minimum-size")))),
			flakes::particle_maximum_size(
				sge::parse::json::find_and_convert_member<cl_float>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("particle-maximum-size")))),
			flakes::snow_texture_size(
				sge::parse::json::find_and_convert_member<sge::renderer::dim2>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("snow-texture-size")))))),
	ground_(
		fcppt::make_unique_ptr<visualization::ground>(
			fcppt::ref(
				_renderer),
			fcppt::cref(
				shape_manager_->vertex_declaration()),
			fcppt::ref(
				_image_system),
			volume::grid_size(
				boundary_->get().size()))),
	wind_blower_(
		fcppt::make_unique_ptr<simulation::stam::wind_blower>(
			fcppt::ref(
				*simulation_),
			simulation::stam::wind_speed_minimum(
				sge::parse::json::find_and_convert_member<cl_float>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("wind-speed-minimum")))),
			simulation::stam::wind_speed_maximum(
				sge::parse::json::find_and_convert_member<cl_float>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("wind-speed-maximum")))),
			simulation::stam::wind_change_frequency(
				sge::parse::json::find_and_convert_member<cl_float>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("wind-change-frequency")))))),
	draw_arrows_(
		false)
{
}

void
flakelib::volume::flake_simulation::update(
	flakelib::duration const &_duration)
{
	simulation_->update(
		_duration);

	flakes_->update(
		_duration,
		simulation_->velocity());

	wind_blower_->update(
		_duration);

	if(draw_arrows_)
		arrows_->convert(
			simulation_->velocity());
}

void
flakelib::volume::flake_simulation::render()
{
	shape_manager_->render(
		camera_.mvp());

	ground_->render(
		camera_.mvp(),
		flakes_->current_snow_texture());

	flakes_->render();

	if(draw_arrows_)
	{
		sge::shader::scoped scoped_shader(
			arrows_manager_->shader(),
			sge::shader::activate_everything());

		arrows_manager_->camera_position(
			camera_.gizmo().position());

		arrows_manager_->shader().update_uniform(
			"mvp",
			sge::shader::matrix(
				camera_.mvp(),
				sge::shader::matrix_flags::projection));

		arrows_->render();
	}
}

void
flakelib::volume::flake_simulation::toggle_arrows()
{
	draw_arrows_ = !draw_arrows_;
}

void
flakelib::volume::flake_simulation::external_force_magnitude(
	cl_float const _external_force)
{
	simulation_->external_force_magnitude(
		_external_force);
}

flakelib::volume::flake_simulation::~flake_simulation()
{
	fcppt::io::cout() << simulation_->parent_profiler() << FCPPT_TEXT("\n");
}
