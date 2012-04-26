#include <flakelib/buffer/linear_view.hpp>
#include <flakelib/buffer_pool/volume_lock.hpp>
#include <flakelib/utility/fill_buffer.hpp>
#include <flakelib/volume/retrieve_filled_float_buffer.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/move.hpp>
#include <fcppt/ref.hpp>


flakelib::volume::unique_float_buffer_lock
flakelib::volume::retrieve_filled_float_buffer(
	buffer_pool::object &_buffer_pool,
	utility::fill_buffer &_fill_buffer,
	sge::opencl::memory_object::dim3 const &_size,
	cl_float const _value)
{
	flakelib::volume::unique_float_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::volume::float_buffer_lock>(
			fcppt::ref(
				_buffer_pool),
			_size));

	_fill_buffer.apply(
		flakelib::buffer::linear_view<cl_float>(
			result->value().buffer()),
		_value);

	return
		fcppt::move(
			result);
}
