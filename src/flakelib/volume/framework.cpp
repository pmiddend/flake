#include <fcppt/io/cout.hpp>
#include <flakelib/volume/visualization/arrows_manager.hpp>
#include <sge/shader/object.hpp>
#include <sge/shader/activate_bare.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/opencl/memory_object/dim3.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <fcppt/ref.hpp>
#include <flakelib/volume/framework.hpp>
#include <flakelib/volume/laplace_solver/jacobi.hpp>
#include <flakelib/volume/simulation/stam/object.hpp>
#include <flakelib/volume/boundary/object.hpp>
#include <flakelib/volume/visualization/shape_manager.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <flakelib/volume/visualization/arrows.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/math/dim/basic_impl.hpp>

flakelib::volume::framework::framework(
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
						FCPPT_TEXT("volume-framework/arrow-grid-scale"))))))
{
	sge::opencl::memory_object::size_type const grid_size =
		boundary_->get().size()[0];

	flakelib::volume::boundary::sphere::object single_sphere(
		flakelib::volume::boundary::sphere::radius(
			grid_size/3),
		flakelib::volume::boundary::sphere::position(
			sge::opencl::memory_object::dim3(
				grid_size/2,
				grid_size/2,
				grid_size/2)));

	boundary_->add(
		single_sphere);

	shape_manager_->add(
		single_sphere);
}

void
flakelib::volume::framework::update(
	flakelib::duration const &_duration)
{
	simulation_->update(
		_duration);

	arrows_->convert(
		simulation_->velocity());
}

void
flakelib::volume::framework::render(
	sge::renderer::matrix4 const &_mvp)
{
	shape_manager_->render(
		_mvp);

	sge::shader::scoped scoped_shader(
		arrows_manager_->shader(),
		sge::shader::activate_bare());

	arrows_manager_->shader().update_uniform(
		"mvp",
		sge::shader::matrix(
			_mvp,
			sge::shader::matrix_flags::projection));

	arrows_->render();
}

flakelib::volume::framework::~framework()
{
	fcppt::io::cout() << simulation_->parent_profiler() << FCPPT_TEXT("\n");
}
