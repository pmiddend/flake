#ifndef FLAKELIB_BUFFER_PLANAR_VIEW_DECL_HPP_INCLUDED
#define FLAKELIB_BUFFER_PLANAR_VIEW_DECL_HPP_INCLUDED

#include <flakelib/buffer/planar_view_fwd.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/memory_object/dim2.hpp>
#include <fcppt/math/dim/object_impl.hpp>

namespace flakelib
{
namespace buffer
{
template<typename T>
class planar_view
{
public:
	typedef
	T
	value_type;

	explicit
	planar_view(
		sge::opencl::memory_object::buffer &,
		sge::opencl::memory_object::dim2 const &);

	sge::opencl::memory_object::buffer &
	buffer() const;

	sge::opencl::memory_object::dim2 const &
	size() const;

	~planar_view();
private:
	sge::opencl::memory_object::buffer *buffer_;
	sge::opencl::memory_object::dim2 size_;
};
}
}

#endif
