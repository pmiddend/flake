#ifndef FLAKELIB_VOLUME_RETRIEVE_FILLED_FLOAT_BUFFER_HPP_INCLUDED
#define FLAKELIB_VOLUME_RETRIEVE_FILLED_FLOAT_BUFFER_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/utility/fill_buffer.hpp>
#include <flakelib/volume/unique_float_buffer_lock.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/memory_object/dim3.hpp>


namespace flakelib
{
namespace volume
{
FLAKELIB_SYMBOL
flakelib::volume::unique_float_buffer_lock
retrieve_filled_float_buffer(
	buffer_pool::object &,
	utility::fill_buffer &,
	sge::opencl::memory_object::dim3 const &,
	cl_float);
}
}

#endif

