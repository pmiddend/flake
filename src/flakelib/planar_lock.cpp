#include <flakelib/planar_cache.hpp>
#include <flakelib/planar_lock.hpp>


flakelib::planar_lock::planar_lock(
	flakelib::planar_cache &_planar_cache,
	sge::opencl::memory_object::size_type const _size)
:
	planar_cache_(
		_planar_cache),
	value_(
		planar_cache_.get(
			_size))
{
	planar_cache_.lock(
		value_);
}

flakelib::planar_lock::planar_lock(
	flakelib::planar_cache &_planar_cache,
	sge::opencl::memory_object::dim2 const &_size)
:
	planar_cache_(
		_planar_cache),
	value_(
		planar_cache_.get(
			_size))
{
	planar_cache_.lock(
		value_);
}

sge::opencl::memory_object::image::planar &
flakelib::planar_lock::value() const
{
	return value_;
}

flakelib::planar_lock::~planar_lock()
{
	planar_cache_.unlock(
		value_);
}
