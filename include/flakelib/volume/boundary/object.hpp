#ifndef FLAKELIB_VOLUME_BOUNDARY_OBJECT_HPP_INCLUDED
#define FLAKELIB_VOLUME_BOUNDARY_OBJECT_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/buffer/volume_view.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/buffer_pool/volume_lock.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <flakelib/volume/boundary/view.hpp>
#include <flakelib/volume/boundary/cube/object_fwd.hpp>
#include <flakelib/volume/boundary/sphere/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/dim3.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace volume
{
namespace boundary
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::opencl::command_queue::object &,
		flakelib::utility::object &,
		flakelib::buffer_pool::object &,
		flakelib::build_options const &,
		sge::opencl::memory_object::dim3 const &);

	boundary::view::value_type const
	get();

	void
	add(
		boundary::sphere::object const &);

	void
	add(
		boundary::cube::object const &);

	~object();
private:
	sge::opencl::command_queue::object &command_queue_;
	flakelib::buffer_pool::volume_lock<cl_float> buffer_lock_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object add_sphere_kernel_;
	sge::opencl::kernel::object add_cube_kernel_;

};
}
}
}

#endif
