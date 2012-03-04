#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_DIVERGENCE_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_DIVERGENCE_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/cl/kernel_fwd.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/planar/boundary_buffer_view.hpp>
#include <flakelib/planar/float2_view.hpp>
#include <flakelib/planar/unique_float_buffer_lock.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace planar
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
	explicit
	divergence(
		cl::program_context const &,
		flakelib::buffer_pool::object &);

	FLAKELIB_SYMBOL
	planar::unique_float_buffer_lock
	update(
		planar::boundary_buffer_view const &,
		planar::float2_view const &);

	FLAKELIB_SYMBOL
	~divergence();
private:
	flakelib::buffer_pool::object &buffer_pool_;
	cl::program program_;
	cl::unique_kernel_ptr kernel_;

};
}
}
}
}

#endif

