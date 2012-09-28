#ifndef FLAKE_VOLUME_TESTS_FLAKES_HPP_INCLUDED
#define FLAKE_VOLUME_TESTS_FLAKES_HPP_INCLUDED

#include <flake/skydome/object.hpp>
#include <flake/test/base.hpp>
#include <flake/test/information/object.hpp>
#include <flake/volume/arrows/manager.hpp>
#include <flake/volume/arrows/object.hpp>
#include <flake/volume/flakes/manager.hpp>
#include <flake/volume/flakes/mover.hpp>
#include <flake/volume/flakes/snow_density_view.hpp>
	//#include <flake/volume/model/manager.hpp>
#include <flake/volume/model/object.hpp>
#include <flake/volume/obstacles/manager.hpp>
#include <flake/volume/snow_cover/object.hpp>
#include <flake/volume/snow_cover/parallel_update.hpp>
#include <flakelib/marching_cubes/cpu/object.hpp>
#include <flakelib/marching_cubes/gpu/manager.hpp>
#include <flakelib/marching_cubes/gpu/object.hpp>
#include <flakelib/marching_cubes/gpu/vertex_count.hpp>
#include <flakelib/scan/object.hpp>
#include <flakelib/splatter/object.hpp>
#include <flakelib/utility/fill_buffer.hpp>
#include <flakelib/value_modulator/object.hpp>
#include <flakelib/volume/gradient.hpp>
#include <flakelib/volume/grid_size.hpp>
#include <flakelib/volume/unique_float4_buffer_lock.hpp>
#include <flakelib/volume/unique_float_buffer_lock.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <flakelib/volume/simulation/stam/divergence.hpp>
#include <flakelib/volume/simulation/stam/jacobi.hpp>
#include <flakelib/volume/simulation/stam/outflow_boundaries.hpp>
#include <flakelib/volume/simulation/stam/semilagrangian_advection.hpp>
#include <flakelib/volume/simulation/stam/subtract_pressure_gradient.hpp>
#include <flakelib/volume/simulation/stam/wind_source.hpp>
#include <sge/camera/perspective_projection_from_viewport.hpp>
#include <sge/camera/first_person/object.hpp>
#include <sge/scenic/scene/manager.hpp>
#include <sge/scenic/scene/object.hpp>
#include <sge/input/keyboard/key_event_fwd.hpp>
#include <sge/opencl/dim3.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/timer/basic.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <awl/main/exit_code.hpp>
#include <awl/main/function_context_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/signal/scoped_connection.hpp>


namespace flake
{
namespace volume
{
namespace tests
{
class flakes
:
	public flake::test::base
{
FCPPT_NONCOPYABLE(
	flakes);
public:
	explicit
	flakes(
		awl::main::function_context const &);

	awl::main::exit_code const
	run();

	~flakes();
private:
	flakelib::volume::grid_size const simulation_size_;
	sge::camera::first_person::object camera_;
	sge::camera::perspective_projection_from_viewport perspective_projection_from_viewport_;
	flakelib::utility::fill_buffer fill_buffer_;
	flakelib::splatter::object splatter_;
	flakelib::volume::conversion::object conversion_object_;
	flake::volume::arrows::manager arrows_manager_;
	flake::volume::arrows::object velocity_arrows_;

	// Simulation
	flakelib::volume::simulation::stam::wind_source wind_source_;
	flakelib::volume::simulation::stam::outflow_boundaries outflow_boundaries_;
	flakelib::volume::simulation::stam::semilagrangian_advection semilagrangian_advection_;
	flakelib::volume::simulation::stam::divergence divergence_;
	flakelib::volume::simulation::stam::jacobi jacobi_;
	flakelib::volume::simulation::stam::subtract_pressure_gradient subtract_pressure_gradient_;

	// Buffers
	flakelib::volume::unique_float_buffer_lock boundary_buffer_;
	sge::opencl::memory_object::buffer snow_density_buffer_;
	flake::volume::flakes::snow_density_view snow_density_view_;
	flakelib::volume::unique_float_buffer_lock activity_buffer_;
	flakelib::volume::unique_float_buffer_lock initial_guess_buffer_;
	flakelib::volume::unique_float4_buffer_lock velocity_buffer_;

	flake::skydome::object skydome_;
	flake::volume::flakes::manager flakes_;
	flake::volume::flakes::mover flakes_mover_;
	sge::scenic::scene::manager scene_manager_;
	sge::scenic::scene::object scene_;
		//flakelib::volume::gradient gradient_;
		//flakelib::scan::object scan_;
		//flakelib::marching_cubes::gpu::manager marching_cubes_manager_;
	flakelib::marching_cubes::cpu::object marching_cubes_manager_;
	flake::volume::snow_cover::object snow_cover_;
	flake::volume::snow_cover::parallel_update snow_cover_parallel_update_;
		//flakelib::marching_cubes::gpu::object marching_cubes_;
	flakelib::value_modulator::object wind_strength_modulator_;

	// Timer
	sge::timer::basic<sge::timer::clocks::standard> delta_timer_;
	sge::timer::basic<sge::timer::clocks::standard> snow_cover_update_;
		//flakelib::marching_cubes::gpu::vertex_count snow_cover_vertices_;
		//flake::test::information::object snow_cover_vertices_information_;

	void
	render(
		sge::renderer::context::ffp &);

	void
	update();
};
}
}
}

#endif

