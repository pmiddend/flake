#ifndef FLAKE_VOLUME_TESTS_SOLIPSISTIC_HPP_INCLUDED
#define FLAKE_VOLUME_TESTS_SOLIPSISTIC_HPP_INCLUDED

#include <flakelib/volume/simulation/stam/divergence.hpp>
#include <flakelib/volume/simulation/stam/jacobi.hpp>
#include <flakelib/utility/fill_buffer.hpp>
#include <flakelib/volume/grid_size.hpp>
#include <flakelib/volume/simulation/stam/outflow_boundaries.hpp>
#include <flakelib/volume/simulation/stam/semilagrangian_advection.hpp>
#include <flakelib/volume/simulation/stam/subtract_pressure_gradient.hpp>
#include <flakelib/volume/simulation/stam/wind_source.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <awl/main/function_context_fwd.hpp>
#include <awl/main/exit_code.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/charconv/system_scoped_ptr.hpp>
#include <sge/parse/json/object.hpp>
#include <flakelib/volume/unique_float4_buffer_lock.hpp>
#include <flakelib/volume/unique_float_buffer_lock.hpp>

namespace flake
{
namespace volume
{
namespace tests
{
class solipsistic
{
FCPPT_NONCOPYABLE(
	solipsistic);
public:
	explicit
	solipsistic(
		awl::main::function_context const &);

	awl::main::exit_code const
	run();

	~solipsistic();
private:
	sge::opencl::single_device_system::object opencl_system_;
	sge::charconv::system_scoped_ptr charconv_system_;
	sge::parse::json::object json_configuration_;
	flakelib::volume::grid_size const simulation_size_;
	flakelib::cl::program_context program_context_;
	flakelib::utility::fill_buffer fill_buffer_;
	flakelib::buffer_pool::object buffer_pool_;

	flakelib::volume::simulation::stam::wind_source wind_source_;
	flakelib::volume::simulation::stam::outflow_boundaries outflow_boundaries_;
	flakelib::volume::simulation::stam::semilagrangian_advection semilagrangian_advection_;
	flakelib::volume::simulation::stam::divergence divergence_;
	flakelib::volume::simulation::stam::jacobi jacobi_;
	flakelib::volume::simulation::stam::subtract_pressure_gradient subtract_pressure_gradient_;

	// Buffers
	flakelib::volume::unique_float_buffer_lock boundary_buffer_;
	flakelib::volume::unique_float4_buffer_lock velocity_buffer_;

	void
	run_once();
};
}
}
}

#endif

