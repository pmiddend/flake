#ifndef FLAKELIB_PLANAR_BUFFER_HPP_INCLUDED
#define FLAKELIB_PLANAR_BUFFER_HPP_INCLUDED

#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/memory_object/dim2.hpp>
#include <fcppt/math/dim/basic_impl.hpp>

namespace flakelib
{
class planar_buffer
{
public:
	explicit
	planar_buffer(
		sge::opencl::memory_object::buffer &,
		sge::opencl::memory_object::dim2 const &);

	sge::opencl::memory_object::buffer &
	buffer() const;

	sge::opencl::memory_object::dim2 const &
	size() const;

	~planar_buffer();
private:
	sge::opencl::memory_object::buffer *buffer_;
	sge::opencl::memory_object::dim2 size_;
};
}

#endif
