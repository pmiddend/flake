#ifndef FLAKELIB_BUFFER_PLANAR_VIEW_IMPL_HPP_INCLUDED
#define FLAKELIB_BUFFER_PLANAR_VIEW_IMPL_HPP_INCLUDED

#include <flakelib/buffer/planar_view_decl.hpp>

template<typename T>
flakelib::buffer::planar_view<T>::planar_view(
	sge::opencl::memory_object::buffer &_buffer,
	sge::opencl::memory_object::dim2 const &_size)
:
	buffer_(
		_buffer),
	size_(
		_size)
{
}

template<typename T>
sge::opencl::memory_object::buffer &
flakelib::buffer::planar_view<T>::buffer() const
{
	return buffer_;
}

template<typename T>
sge::opencl::memory_object::dim2 const &
flakelib::buffer::planar_view<T>::size() const
{
	return size_;
}

template<typename T>
flakelib::buffer::planar_view<T>::~planar_view()
{
}

#endif
