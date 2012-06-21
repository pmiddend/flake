#ifndef FLAKELIB_BUFFER_LINEAR_VIEW_IMPL_HPP_INCLUDED
#define FLAKELIB_BUFFER_LINEAR_VIEW_IMPL_HPP_INCLUDED

#include <flakelib/buffer/linear_view_decl.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <cstdlib>
#include <fcppt/config/external_end.hpp>


template<typename T>
flakelib::buffer::linear_view<T>::linear_view(
	sge::opencl::memory_object::buffer &_buffer)
:
	buffer_(
		_buffer)
{
	FCPPT_ASSERT_PRE(
		static_cast<std::size_t>(buffer_.byte_size().get()) % sizeof(T) == 0);
}

template<typename T>
sge::opencl::memory_object::buffer &
flakelib::buffer::linear_view<T>::buffer() const
{
	return buffer_;
}

template<typename T>
sge::opencl::dim1
flakelib::buffer::linear_view<T>::size() const
{
	return
		sge::opencl::dim1(
			buffer_.byte_size().get() / sizeof(T));
}

template<typename T>
flakelib::buffer::linear_view<T>::~linear_view()
{
}

#endif
