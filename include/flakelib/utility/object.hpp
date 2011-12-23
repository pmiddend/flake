#ifndef FLAKELIB_UTILITY_OBJECT_HPP_INCLUDED
#define FLAKELIB_UTILITY_OBJECT_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/buffer/planar_view_fwd.hpp>
#include <flakelib/buffer/linear_view_fwd.hpp>
#include <flakelib/utility/copy_from.hpp>
#include <flakelib/utility/copy_to.hpp>
#include <flakelib/utility/multiplier.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
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
		sge::opencl::command_queue::object &,
		flakelib::build_options const &);

	void
	null_buffer(
		buffer::linear_view<cl_float> const &);

	void
	copy_buffer(
		utility::copy_from const &,
		utility::copy_to const &,
		utility::multiplier const &);

	void
	generate_oscillation(
		buffer::planar_view<cl_float> const &);

	/*
	void
	planar_vector_magnitude(
		utility::from const &,
		utility::to const &,
		utility::multiplier const &);
	*/

	cl_float
	frobenius_norm(
		buffer::linear_view<cl_float> const &);

	~object();
private:
	sge::opencl::command_queue::object &command_queue_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object copy_float_buffer_kernel_;
	sge::opencl::kernel::object null_float_buffer_kernel_;
	sge::opencl::kernel::object generate_float_oscillation_kernel_;
	sge::opencl::kernel::object frobenius_norm_tile_kernel_;
	sge::opencl::kernel::object planar_vector_magnitude_kernel_;
};
}
}

#endif
