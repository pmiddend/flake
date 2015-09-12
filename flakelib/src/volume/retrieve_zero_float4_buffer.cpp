#include <flakelib/buffer/linear_view.hpp>
#include <flakelib/buffer_pool/volume_lock.hpp>
#include <flakelib/utility/fill_buffer.hpp>
#include <flakelib/volume/retrieve_zero_float4_buffer.hpp>
#include <fcppt/make_unique_ptr.hpp>


flakelib::volume::unique_float4_buffer_lock
flakelib::volume::retrieve_zero_float4_buffer(
	buffer_pool::object &_buffer_pool,
	utility::fill_buffer &_fill_buffer,
	sge::opencl::dim3 const &_size)
{
	flakelib::volume::unique_float4_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::volume::float4_buffer_lock>(
			_buffer_pool,
			_size));

	_fill_buffer.apply(
		flakelib::buffer::linear_view<cl_float>(
			result->value().buffer()),
		static_cast<cl_float>(
			0.0f));

	return
		result;
}
