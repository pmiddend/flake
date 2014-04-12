#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_VORTICITY_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_VORTICITY_HPP_INCLUDED

#include <flakelib/duration.hpp>
#include <flakelib/symbol.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/cl/kernel_unique_ptr.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/volume/boundary_buffer_view.hpp>
#include <flakelib/volume/float_view.hpp>
#include <flakelib/volume/unique_float4_buffer_lock.hpp>
#include <flakelib/volume/unique_float_buffer_lock.hpp>
#include <flakelib/volume/simulation/stam/velocity.hpp>
#include <flakelib/volume/simulation/stam/vorticity_strength.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace volume
{
namespace simulation
{
namespace stam
{
class vorticity
{
FCPPT_NONCOPYABLE(
	vorticity);
public:
	FLAKELIB_SYMBOL
	vorticity(
		cl::program_context const &,
		flakelib::buffer_pool::object &);

	FLAKELIB_SYMBOL
	volume::unique_float4_buffer_lock
	apply_vorticity(
		volume::boundary_buffer_view const &,
		stam::velocity const &);

	FLAKELIB_SYMBOL
	volume::unique_float4_buffer_lock
	apply_confinement(
		volume::float4_view const &,
		stam::velocity const &,
		flakelib::duration const &,
		stam::vorticity_strength const &);

	FLAKELIB_SYMBOL
	~vorticity();
private:
	flakelib::buffer_pool::object &buffer_pool_;
	cl::program program_;
	cl::kernel_unique_ptr const vorticity_kernel_;
	cl::kernel_unique_ptr const confinement_kernel_;
};
}
}
}
}

#endif

