#ifndef FLAKELIB_DENSITY_ADVECTOR_HPP_INCLUDED
#define FLAKELIB_DENSITY_ADVECTOR_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer/planar_view.hpp>
#include <flakelib/buffer_pool/planar_lock.hpp>
#include <flakelib/density/grid_dimensions.hpp>
#include <flakelib/density/grid_scale.hpp>
#include <flakelib/density/velocity_image.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/unique_ptr.hpp>


namespace flakelib
{
namespace density
{
class advector
{
FCPPT_NONCOPYABLE(
	advector);
public:
	explicit
	advector(
		sge::opencl::command_queue::object &,
		buffer_pool::object &,
		utility::object &,
		flakelib::build_options const &,
		density::grid_dimensions const &,
		density::grid_scale const &);

	void
	update(
		density::velocity_image const &,
		flakelib::duration const &);

	buffer::planar_view<cl_float> const
	density_image();

	buffer::planar_view<cl_float> const
	source_image();

	~advector();
private:
	typedef
	fcppt::unique_ptr
	<
		buffer_pool::planar_lock<cl_float>
	>
	unique_planar_float_lock;

	sge::opencl::command_queue::object &command_queue_;
	buffer_pool::object &buffer_pool_;
	cl_float const grid_scale_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object apply_sources_kernel_;
	sge::opencl::kernel::object advect_kernel_;
	buffer_pool::planar_lock<cl_float> sources_;
	// The current density always changes (since it's advected and thereby
	// moved to a different container). The source is modified, but
	// in-place, and is thus fixed.
	unique_planar_float_lock current_density_;
};
}
}

#endif
