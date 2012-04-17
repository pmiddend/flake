#ifndef FLAKELIB_VOLUME_GRADIENT_HPP_INCLUDED
#define FLAKELIB_VOLUME_GRADIENT_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/cl/kernel_fwd.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/volume/float_view.hpp>
#include <flakelib/volume/unique_float4_buffer_lock.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace volume
{
class gradient
{
FCPPT_NONCOPYABLE(
	gradient);
public:
	FLAKELIB_SYMBOL
	gradient(
		cl::program_context const &,
		flakelib::buffer_pool::object &);

	FLAKELIB_SYMBOL
	volume::unique_float4_buffer_lock
	update(
		volume::float_view const &);

	FLAKELIB_SYMBOL
	~gradient();
private:
	flakelib::buffer_pool::object &buffer_pool_;
	cl::program program_;
	cl::unique_kernel_ptr kernel_;

};
}
}

#endif

