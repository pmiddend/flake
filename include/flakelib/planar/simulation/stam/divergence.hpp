#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_DIVERGENCE_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_DIVERGENCE_HPP_INCLUDED

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
	explicit
	divergence(
		program::context const &,
		flakelib::buffer_pool::object &);

	planar::unique_float_buffer_lock
	update(
		planar::boundary_buffer_view const &,
		planar::float2_view const &);

	~divergence();
private:
	flakelib::buffer_pool::object &buffer_pool_;
	program::unique_ptr program_;
	sge::opencl::kernel::object advect_kernel_;

};
}
}
}
}

#endif

