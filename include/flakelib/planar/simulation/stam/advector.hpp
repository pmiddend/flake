#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_ADVECTOR_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_ADVECTOR_HPP_INCLUDED

#include <flakelib/duration.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace planar
{
namespace simulation
{
namespace stam
{
class advector
{
FCPPT_NONCOPYABLE(
	advector);
public:
	explicit
	advector(
		sge::opencl::command_queue::object &,
		flakelib::planar::boundary_image_view const &,
		flakelib::build_options const &);

	void
	update(
		flakelib::duration const &);

	~advector();
private:
	sge::opencl::program::object program_;
	sge::opencl::kernel::object advect_kernel_;
};
}
}
}
}

#endif

