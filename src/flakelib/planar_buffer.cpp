#include <flakelib/planar_buffer.hpp>

flakelib::planar_buffer::planar_buffer(
	sge::opencl::memory_object::buffer &_buffer,
	sge::opencl::memory_object::dim2 const &_size)
:
	buffer_(
		&_buffer),
	size_(
		_size)
{
}

sge::opencl::memory_object::buffer &
flakelib::planar_buffer::buffer() const
{
	return *buffer_;
}

sge::opencl::memory_object::dim2 const &
flakelib::planar_buffer::size() const
{
	return size_;
}

flakelib::planar_buffer::~planar_buffer()
{
}
