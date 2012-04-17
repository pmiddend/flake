#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_DIVERGENCE_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_DIVERGENCE_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/cl/kernel_scoped_ptr.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/volume/boundary_buffer_view.hpp>
#include <flakelib/volume/float4_view.hpp>
#include <flakelib/volume/unique_float_buffer_lock.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace volume
{
namespace simulation
{
namespace stam
{
class divergence
{
FCPPT_NONCOPYABLE(
	divergence);
public:
	FLAKELIB_SYMBOL
	divergence(
		cl::program_context const &,
		flakelib::buffer_pool::object &);

	FLAKELIB_SYMBOL
	volume::unique_float_buffer_lock
	update(
		volume::boundary_buffer_view const &,
		volume::float4_view const &);

	FLAKELIB_SYMBOL
	~divergence();
private:
	flakelib::buffer_pool::object &buffer_pool_;
	cl::program program_;
	cl::kernel_scoped_ptr kernel_;

};
}
}
}
}

#endif

