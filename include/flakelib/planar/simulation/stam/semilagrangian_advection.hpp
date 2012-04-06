#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_SEMILAGRANGIAN_ADVECTION_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_SEMILAGRANGIAN_ADVECTION_HPP_INCLUDED

#include <flakelib/duration.hpp>
#include <flakelib/symbol.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/planar/boundary_buffer_view.hpp>
#include <flakelib/planar/float2_view.hpp>
#include <flakelib/planar/float_view.hpp>
#include <flakelib/planar/unique_float2_buffer_lock.hpp>
#include <flakelib/planar/unique_float_buffer_lock.hpp>
#include <flakelib/planar/simulation/stam/velocity.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace planar
{
namespace simulation
{
namespace stam
{
class semilagrangian_advection
{
FCPPT_NONCOPYABLE(
	semilagrangian_advection);
public:
	FLAKELIB_SYMBOL
	semilagrangian_advection(
		cl::program_context const &,
		flakelib::buffer_pool::object &);

	FLAKELIB_SYMBOL
	planar::unique_float2_buffer_lock
	update_float2(
		planar::boundary_buffer_view const &,
		stam::velocity const &,
		planar::float2_view const &,
		flakelib::duration const &);

	FLAKELIB_SYMBOL
	planar::unique_float_buffer_lock
	update_float(
		planar::boundary_buffer_view const &,
		stam::velocity const &,
		planar::float_view const &,
		flakelib::duration const &);

	FLAKELIB_SYMBOL
	~semilagrangian_advection();
private:
	flakelib::buffer_pool::object &buffer_pool_;
	cl::program program_;
	cl::unique_kernel_ptr planar_kernel_;
	cl::unique_kernel_ptr scalar_kernel_;
};
}
}
}
}

#endif

