#ifndef FLAKELIB_VOLUME_DENSITY_ADVECTOR_HPP_INCLUDED
#define FLAKELIB_VOLUME_DENSITY_ADVECTOR_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer/volume_view.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/buffer_pool/volume_lock.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <flakelib/volume/boundary/view.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/timer/basic.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/unique_ptr.hpp>


namespace flakelib
{
namespace volume
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
		volume::boundary::view const &,
		flakelib::build_options const &,
		buffer_pool::object &,
		utility::object &);

	void
	update(
		buffer::volume_view<cl_float4> const &,
		flakelib::duration const &);

	buffer::volume_view<cl_float> const
	get();

	~advector();
private:
	typedef
	flakelib::buffer_pool::volume_lock<cl_float>
	volume_float_lock;

	typedef
	fcppt::unique_ptr<volume_float_lock>
	unique_volume_float_lock;

	sge::opencl::command_queue::object &command_queue_;
	buffer_pool::object &buffer_pool_;
	sge::timer::basic<sge::timer::clocks::standard> density_strength_timer_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object apply_sources_kernel_;
	sge::opencl::kernel::object advect_kernel_;
	volume_float_lock sources_;
	// The current density always changes (since it's advected and thereby
	// moved to a different container). The source is modified, but
	// in-place, and is thus fixed.
	unique_volume_float_lock current_density_;
};
}
}
}

#endif
