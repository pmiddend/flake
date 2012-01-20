#include <fcppt/cref.hpp>
#include <flakelib/volume/visualization/ground.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/utility/object.hpp>
#include <flakelib/volume/smoke_simulation.hpp>
#include <flakelib/volume/boundary/object.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <flakelib/volume/density/advector.hpp>
#include <flakelib/volume/density/visual.hpp>
#include <flakelib/volume/flakes/object.hpp>
#include <flakelib/volume/laplace_solver/jacobi.hpp>
#include <flakelib/volume/simulation/stam/object.hpp>
#include <flakelib/volume/visualization/arrows.hpp>
#include <flakelib/volume/visualization/arrows_manager.hpp>
#include <flakelib/volume/visualization/shape_manager.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/dim3.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/object.hpp>
#include <sge/shader/scoped.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/math/dim/basic_impl.hpp>


flakelib::volume::smoke_simulation::smoke_simulation(
	sge::opencl::command_queue::object &_command_queue,
	sge::renderer::device &_renderer,
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
			sge::parse::json::find_and_convert_member<sge::opencl::memory_object::dim3>(
				_json_config,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("volume-framework/grid-size"))))),
	laplace_solver_(
		fcppt::make_unique_ptr<laplace_solver::jacobi>(
			fcppt::ref(
				*buffer_pool_),
			fcppt::ref(
				_command_queue),
			_build_options,
			flakelib::volume::laplace_solver::boundary(
				boundary_->get()),
			flakelib::volume::laplace_solver::grid_scale(
				1.0f),
			flakelib::volume::laplace_solver::iterations(
				sge::parse::json::find_and_convert_member<unsigned>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("volume-framework/jacobi-iterations")))))),
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
						FCPPT_TEXT("volume-framework/external-force-magnitude")))),
			simulation::stam::grid_scale(
				1.0f),
			simulation::stam::profiling_enabled(
				sge::parse::json::find_and_convert_member<bool>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("volume-framework/profiling-enabled")))),
			_build_options,
			fcppt::ref(
				*buffer_pool_),
			fcppt::ref(
				*utility_),
			fcppt::ref(
				*laplace_solver_))),
	shape_manager_(
		fcppt::make_unique_ptr<visualization::shape_manager>(
			fcppt::ref(
				_renderer),
			fcppt::ref(
				_image_system))),
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
						FCPPT_TEXT("volume-framework/arrow-scale")))),
			conversion::grid_scale(
				sge::parse::json::find_and_convert_member<cl_float>(
					_json_config,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("volume-framework/arrow-grid-scale")))))),
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
				*utility_),
			density::grid_scale(
				1.0f))),
	density_visual_(
		fcppt::make_unique_ptr<density::visual>(
			fcppt::ref(
				_renderer),
			fcppt::ref(
				_image_system),
			fcppt::ref(
				_command_queue),
			_build_options,
			density::grid_size(
				boundary_->get().size())))
{
	sge::opencl::memory_object::size_type const grid_size =
		boundary_->get().size()[0];

	flakelib::volume::boundary::sphere::object single_sphere(
		flakelib::volume::boundary::sphere::radius(
			grid_size/4),
		flakelib::volume::boundary::sphere::position(
			sge::opencl::memory_object::dim3(
				grid_size/2,
				grid_size/2,
				grid_size/2)));

	flakelib::volume::boundary::cube::object companion_cube(
		flakelib::volume::boundary::cube::width(
			20),
		flakelib::volume::boundary::cube::position(
			sge::opencl::memory_object::dim3(
				20,
				0,
				20)));

	boundary_->add(
		companion_cube);

	shape_manager_->add(
		companion_cube);
	/*
	boundary_->add(
		single_sphere);

	shape_manager_->add(
		single_sphere);
		*/
}

void
flakelib::volume::smoke_simulation::update(
	flakelib::duration const &_duration)
{
	simulation_->update(
		_duration);

	arrows_->convert(
		simulation_->velocity());

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
	shape_manager_->render(
		_mvp);

	if(false)
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
flakelib::volume::smoke_simulation::external_force_magnitude(
	cl_float const _external_force)
{
	simulation_->external_force_magnitude(
		_external_force);
}

flakelib::volume::smoke_simulation::~smoke_simulation()
{
	fcppt::io::cout() << simulation_->parent_profiler() << FCPPT_TEXT("\n");
}
