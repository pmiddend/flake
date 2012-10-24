#ifndef FLAKELIB_BUFFER_LINEAR_VIEW_DECL_HPP_INCLUDED
#define FLAKELIB_BUFFER_LINEAR_VIEW_DECL_HPP_INCLUDED

#include <flakelib/buffer/linear_view_fwd.hpp>
#include <sge/opencl/dim1.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <fcppt/nonassignable.hpp>


namespace flakelib
{
namespace buffer
{
template<typename T>
class linear_view
{
FCPPT_NONASSIGNABLE(
	linear_view);
public:
	typedef
	T
	value_type;

	explicit
	linear_view(
		sge::opencl::memory_object::buffer &);

	sge::opencl::memory_object::buffer &
	buffer() const;

	sge::opencl::dim1
	size() const;

	~linear_view();
private:
	sge::opencl::memory_object::buffer &buffer_;
};
}
}

#endif
