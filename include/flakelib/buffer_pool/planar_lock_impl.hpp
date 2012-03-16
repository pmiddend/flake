#ifndef FLAKELIB_BUFFER_POOL_PLANAR_LOCK_IMPL_HPP_INCLUDED
#define FLAKELIB_BUFFER_POOL_PLANAR_LOCK_IMPL_HPP_INCLUDED

#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/buffer_pool/planar_lock_decl.hpp>


template<typename T>
flakelib::buffer_pool::planar_lock<T>::planar_lock(
	buffer_pool::object &_pool,
	sge::opencl::memory_object::dim2 const &_size)
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
flakelib::buffer::planar_view<T> const &
flakelib::buffer_pool::planar_lock<T>::value() const
{
	return value_;
}

template<typename T>
flakelib::buffer_pool::planar_lock<T>::~planar_lock()
{
	pool_.unlock(
		value_.buffer());
}

#endif
