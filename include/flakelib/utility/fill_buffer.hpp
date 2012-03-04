#ifndef FLAKELIB_UTILITY_FILL_BUFFER_HPP_INCLUDED
#define FLAKELIB_UTILITY_FILL_BUFFER_HPP_INCLUDED

#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/buffer/linear_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace utility
{
class fill_buffer
{
FCPPT_NONCOPYABLE(
	fill_buffer);
public:
	explicit
	fill_buffer(
		cl::program_context const &);

	void
	apply(
		buffer::linear_view<cl_float> const &,
		cl_float);

	~fill_buffer();
private:
	cl::program program_;
	cl::unique_kernel_ptr kernel_;
};
}
}

#endif

