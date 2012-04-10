#ifndef FLAKE_VOLUME_TESTS_FLAKES_HPP_INCLUDED
#define FLAKE_VOLUME_TESTS_FLAKES_HPP_INCLUDED

#include <flake/test_base.hpp>
#include <flake/volume/arrows/manager.hpp>
#include <flake/volume/arrows/object.hpp>
#include <flake/volume/flakes/manager.hpp>
#include <flakelib/splatter/object.hpp>
#include <flakelib/utility/fill_buffer.hpp>
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
#include <sge/opencl/memory_object/dim3.hpp>
#include <sge/timer/basic.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <awl/main/exit_code.hpp>
#include <awl/main/function_context_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/math/dim/object_impl.hpp>


namespace flake
{
namespace volume
{
namespace tests
{
class flakes
:
	public flake::test_base
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
	// Data
	flakelib::volume::grid_size const simulation_size_;

	// Camera
	sge::camera::first_person::object camera_;
	sge::camera::perspective_projection_from_viewport perspective_projection_from_viewport_;

	// Other stuff
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
	flakelib::volume::unique_float4_buffer_lock velocity_buffer_;
	flakelib::utility::fill_buffer fill_buffer_;

	flake::volume::flakes::manager flakes_;

	// Timer
	sge::timer::basic<sge::timer::clocks::standard> delta_timer_;

	void
	render();

	void
	update();
};
}
}
}

#endif
