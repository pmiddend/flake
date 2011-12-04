#ifndef FLAKELIB_DENSITY_ADVECTOR_HPP_INCLUDED
#define FLAKELIB_DENSITY_ADVECTOR_HPP_INCLUDED

#include <flakelib/duration.hpp>
#include <flakelib/density/grid_dimensions.hpp>
#include <flakelib/density/grid_scale.hpp>
#include <flakelib/planar_pool/object_fwd.hpp>
#include <flakelib/planar_pool/scoped_lock.hpp>
#include <flakelib/planar_pool/unique_lock.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>


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
		planar_pool::object &,
		utility::object &,
		density::grid_dimensions const &,
		density::grid_scale const &);

	void
	update(
		sge::opencl::memory_object::image::planar &,
		flakelib::duration const &);

	sge::opencl::memory_object::image::planar &
	density_image();

	sge::opencl::memory_object::image::planar &
	source_image();

	~advector();
private:
	sge::opencl::command_queue::object &command_queue_;
	planar_pool::object &image_pool_;
	cl_float const grid_scale_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object apply_sources_kernel_;
	sge::opencl::kernel::object advect_kernel_;
	planar_pool::scoped_lock sources_;
	// The current density always changes (since it's advected and thereby
	// moved to a different container). The source is modified, but
	// in-place, and is thus fixed.
	planar_pool::unique_lock current_density_;
};
}
}

#endif
