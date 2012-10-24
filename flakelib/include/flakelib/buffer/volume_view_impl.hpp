#ifndef FLAKELIB_BUFFER_VOLUME_VIEW_IMPL_HPP_INCLUDED
#define FLAKELIB_BUFFER_VOLUME_VIEW_IMPL_HPP_INCLUDED

#include <flakelib/buffer/volume_view_decl.hpp>

template<typename T>
flakelib::buffer::volume_view<T>::volume_view(
	sge::opencl::memory_object::buffer &_buffer,
	sge::opencl::dim3 const &_size)
:
	buffer_(
		_buffer),
	size_(
		_size)
{
}

template<typename T>
sge::opencl::memory_object::buffer &
flakelib::buffer::volume_view<T>::buffer() const
{
	return buffer_;
}

template<typename T>
sge::opencl::dim3 const &
flakelib::buffer::volume_view<T>::size() const
{
	return size_;
}

template<typename T>
flakelib::buffer::volume_view<T>::~volume_view()
{
}

#endif
