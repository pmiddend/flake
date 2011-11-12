#ifndef FLAKELIB_UTILITY_OBJECT_HPP_INCLUDED
#define FLAKELIB_UTILITY_OBJECT_HPP_INCLUDED

#include <flakelib/utility/from.hpp>
#include <flakelib/utility/to.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace utility
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::opencl::command_queue::object &);

	void
	null_image(
		sge::opencl::memory_object::image::planar &);

	void
	copy_image(
		utility::from const &,
		utility::to const &);

	void
	generate_oscillation(
		sge::opencl::memory_object::image::planar &);

	cl_float
	frobenius_norm(
		sge::opencl::memory_object::image::planar &);

	~object();
private:
	sge::opencl::command_queue::object &command_queue_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object copy_image_kernel_;
	sge::opencl::kernel::object null_image_kernel_;
	sge::opencl::kernel::object generate_oscillation_kernel_;
};
}
}

#endif
