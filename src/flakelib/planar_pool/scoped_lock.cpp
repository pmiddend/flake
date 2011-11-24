#include <flakelib/planar_pool/object.hpp>
#include <flakelib/planar_pool/scoped_lock.hpp>


flakelib::planar_pool::scoped_lock::scoped_lock(
	flakelib::planar_pool::object &_planar_cache,
	sge::opencl::memory_object::size_type const _size)
:
	pool_(
		_planar_cache),
	value_(
		pool_.get(
			_size))
{
	pool_.lock(
		value_);
}

flakelib::planar_pool::scoped_lock::scoped_lock(
	flakelib::planar_pool::object &_planar_cache,
	sge::opencl::memory_object::dim2 const &_size)
:
	pool_(
		_planar_cache),
	value_(
		pool_.get(
			_size))
{
	pool_.lock(
		value_);
}

sge::opencl::memory_object::image::planar &
flakelib::planar_pool::scoped_lock::value() const
{
	return value_;
}

flakelib::planar_pool::scoped_lock::~scoped_lock()
{
	pool_.unlock(
		value_);
}
