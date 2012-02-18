#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_SEMILAGRANGIAN_ADVECTION_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_SEMILAGRANGIAN_ADVECTION_HPP_INCLUDED

#include <sge/opencl/kernel/object.hpp>
#include <flakelib/planar/unique_float2_buffer_lock.hpp>
#include <flakelib/planar/float2_view.hpp>
#include <flakelib/planar/boundary_buffer_view.hpp>
#include <flakelib/profiler/parent.hpp>
#include <flakelib/program/context.hpp>
#include <flakelib/program/unique_ptr.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
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
	explicit
	semilagrangian_advection(
		program::context const &,
		flakelib::buffer_pool::object &);

	planar::unique_float2_buffer_lock
	update(
		planar::boundary_buffer_view const &,
		planar::float2_view const &,
		flakelib::duration const &);

	~semilagrangian_advection();
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

