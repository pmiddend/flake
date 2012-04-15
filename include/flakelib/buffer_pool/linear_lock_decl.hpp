#ifndef FLAKELIB_BUFFER_POOL_LINEAR_LOCK_DECL_HPP_INCLUDED
#define FLAKELIB_BUFFER_POOL_LINEAR_LOCK_DECL_HPP_INCLUDED

#include <flakelib/buffer/linear_view.hpp>
#include <flakelib/buffer_pool/linear_lock_fwd.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <sge/opencl/memory_object/dim2.hpp>
#include <fcppt/nonassignable.hpp>


namespace flakelib
{
namespace buffer_pool
{
template<typename T>
class linear_lock
{
FCPPT_NONASSIGNABLE(
	linear_lock);
public:
	linear_lock(
		buffer_pool::object &,
		sge::opencl::memory_object::dim1 const &);

	buffer::linear_view<T> const &
	value() const;

	~linear_lock();
private:
	buffer_pool::object &pool_;
	buffer::linear_view<T> const value_;
};
}
}

#endif

