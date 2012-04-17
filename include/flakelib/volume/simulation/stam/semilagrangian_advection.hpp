#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_SEMILAGRANGIAN_ADVECTION_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_SEMILAGRANGIAN_ADVECTION_HPP_INCLUDED

#include <flakelib/duration.hpp>
#include <flakelib/symbol.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/cl/kernel_scoped_ptr.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/volume/boundary_buffer_view.hpp>
#include <flakelib/volume/float4_view.hpp>
#include <flakelib/volume/float_view.hpp>
#include <flakelib/volume/unique_float4_buffer_lock.hpp>
#include <flakelib/volume/unique_float_buffer_lock.hpp>
#include <flakelib/volume/simulation/stam/velocity.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace volume
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
	volume::unique_float4_buffer_lock
	update_float4(
		volume::boundary_buffer_view const &,
		stam::velocity const &,
		volume::float4_view const &,
		flakelib::duration const &);

	FLAKELIB_SYMBOL
	volume::unique_float_buffer_lock
	update_float(
		volume::boundary_buffer_view const &,
		stam::velocity const &,
		volume::float_view const &,
		flakelib::duration const &);

	FLAKELIB_SYMBOL
	~semilagrangian_advection();
private:
	flakelib::buffer_pool::object &buffer_pool_;
	cl::program program_;
	cl::kernel_scoped_ptr volume_kernel_;
	cl::kernel_scoped_ptr scalar_kernel_;
};
}
}
}
}

#endif

