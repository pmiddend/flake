#ifndef FLAKE_PLANAR_TESTS_SIMPLE_HPP_INCLUDED
#define FLAKE_PLANAR_TESTS_SIMPLE_HPP_INCLUDED

#include <sge/image2d/file_ptr.hpp>
#include <flake/test_base.hpp>
#include <flake/planar/monitor/parent.hpp>
#include <flake/planar/monitor/planar_converter.hpp>
#include <flake/planar/monitor/planar_arrows.hpp>
#include <awl/main/function_context_fwd.hpp>
#include <awl/main/exit_code.hpp>
#include <fcppt/noncopyable.hpp>
#include <flakelib/planar/float_buffer_lock.hpp>
#include <flakelib/planar/simulation/stam/wind_source.hpp>
#include <flakelib/planar/simulation/stam/outflow_boundaries.hpp>
#include <flakelib/planar/simulation/stam/semilagrangian_advection.hpp>
#include <flakelib/utility/fill_buffer.hpp>
#include <flakelib/planar/simulation/stam/divergence.hpp>
#include <flakelib/planar/unique_float2_buffer_lock.hpp>
#include <flakelib/planar/simulation/stam/subtract_pressure_gradient.hpp>
#include <flakelib/planar/simulation/stam/jacobi.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/buffer_pool/planar_lock_impl.hpp>
#include <rucksack/widget/viewport_adaptor.hpp>
#include <rucksack/widget/enumeration.hpp>
#include <sge/timer/basic.hpp>
#include <sge/timer/clocks/standard.hpp>

namespace flake
{
namespace planar
{
namespace tests
{
class simple
:
	flake::test_base
{
FCPPT_NONCOPYABLE(
	simple);
public:
	explicit
	simple(
		awl::main::function_context const &);

	awl::main::exit_code const
	run();

	~simple();
private:
	// Programs
	flakelib::utility::fill_buffer fill_buffer_;
	flakelib::planar::simulation::stam::wind_source wind_source_;
	flakelib::planar::simulation::stam::outflow_boundaries outflow_boundaries_;
	flakelib::planar::simulation::stam::semilagrangian_advection semilagrangian_advection_;
	flakelib::planar::simulation::stam::divergence divergence_;
	flakelib::planar::simulation::stam::jacobi jacobi_;
	flakelib::planar::simulation::stam::subtract_pressure_gradient subtract_pressure_gradient_;
	// Buffers
	sge::image2d::file_ptr boundary_image_file_;
	flakelib::planar::float_buffer_lock boundary_buffer_;
	flakelib::planar::unique_float2_buffer_lock velocity_buffer_;

	// Monitors
	flake::planar::monitor::parent monitor_parent_;
	flake::planar::monitor::planar_converter monitor_planar_converter_;
	flake::planar::monitor::planar_arrows velocity_arrows_;
	rucksack::widget::viewport_adaptor rucksack_viewport_adaptor_;
	rucksack::widget::enumeration rucksack_enumeration_;

	// Timer
	sge::timer::basic<sge::timer::clocks::standard> delta_timer_;
	float const clock_multiplier_;

	void
	render();

	void
	update();
};
}
}
}

#endif

