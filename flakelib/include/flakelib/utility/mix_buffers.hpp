#ifndef FLAKELIB_UTILITY_MIX_BUFFERS_HPP_INCLUDED
#define FLAKELIB_UTILITY_MIX_BUFFERS_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/buffer/linear_view_fwd.hpp>
#include <flakelib/cl/kernel_unique_ptr.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace utility
{
class mix_buffers
{
FCPPT_NONCOPYABLE(
	mix_buffers);
public:
	FLAKELIB_SYMBOL
	explicit
	mix_buffers(
		cl::program_context const &);

	FLAKELIB_SYMBOL
	void
	add_from_to(
		buffer::linear_view<cl_float> const &,
		buffer::linear_view<cl_float> const &);

	FLAKELIB_SYMBOL
	~mix_buffers();
private:
	cl::program program_;
	cl::kernel_unique_ptr const add_kernel_;
};
}
}

#endif

