#ifndef FLAKELIB_BUFFER_POOL_VOLUME_LOCK_IMPL_HPP_INCLUDED
#define FLAKELIB_BUFFER_POOL_VOLUME_LOCK_IMPL_HPP_INCLUDED

#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/buffer_pool/volume_lock_decl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <iostream>
#include <fcppt/config/external_end.hpp>


template<typename T>
flakelib::buffer_pool::volume_lock<T>::volume_lock(
	buffer_pool::object &_pool,
	sge::opencl::dim3 const &_size)
:
	pool_(
		_pool),
	value_(
		_pool.get_and_lock(
			sge::opencl::memory_object::byte_size(
				static_cast<sge::opencl::memory_object::byte_size::value_type>(
					static_cast<sge::opencl::memory_object::byte_size::value_type>(
						_size.content()) *
					static_cast<sge::opencl::memory_object::byte_size::value_type>(
						sizeof(T))))),
		_size)
{
}

template<typename T>
flakelib::buffer::volume_view<T> const &
flakelib::buffer_pool::volume_lock<T>::value() const
{
	return value_;
}

template<typename T>
flakelib::buffer_pool::volume_lock<T>::~volume_lock()
{
	pool_.unlock(
		value_.buffer());
}

#endif
