#ifndef FLAKELIB_BUFFER_VOLUME_VIEW_DECL_HPP_INCLUDED
#define FLAKELIB_BUFFER_VOLUME_VIEW_DECL_HPP_INCLUDED

#include <flakelib/buffer/volume_view_fwd.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/memory_object/dim3.hpp>
#include <fcppt/nonassignable.hpp>
#include <fcppt/math/dim/basic_impl.hpp>

namespace flakelib
{
namespace buffer
{
template<typename T>
class volume_view
{
FCPPT_NONASSIGNABLE(
	volume_view);
public:
	typedef
	T
	value_type;

	explicit
	volume_view(
		sge::opencl::memory_object::buffer &,
		sge::opencl::memory_object::dim3 const &);

	sge::opencl::memory_object::buffer &
	buffer() const;

	sge::opencl::memory_object::dim3 const &
	size() const;

	~volume_view();
private:
	sge::opencl::memory_object::buffer &buffer_;
	sge::opencl::memory_object::dim3 size_;
};
}
}

#endif