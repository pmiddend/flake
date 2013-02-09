#include <flakelib/buffer/linear_view.hpp>
#include <flakelib/buffer_pool/volume_lock.hpp>
#include <flakelib/utility/fill_buffer.hpp>
#include <flakelib/volume/retrieve_filled_float_buffer.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/config/external_begin.hpp>
#include <utility>
#include <fcppt/config/external_end.hpp>


flakelib::volume::unique_float_buffer_lock
flakelib::volume::retrieve_filled_float_buffer(
	buffer_pool::object &_buffer_pool,
	utility::fill_buffer &_fill_buffer,
	sge::opencl::dim3 const &_size,
	cl_float const _value)
{
	flakelib::volume::unique_float_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::volume::float_buffer_lock>(
			_buffer_pool,
			_size));

	_fill_buffer.apply(
		flakelib::buffer::linear_view<cl_float>(
			result->value().buffer()),
		_value);

	return
		std::move(
			result);
}
