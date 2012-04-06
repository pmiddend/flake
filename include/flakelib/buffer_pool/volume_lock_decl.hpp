#ifndef FLAKELIB_BUFFER_POOL_VOLUME_LOCK_DECL_HPP_INCLUDED
#define FLAKELIB_BUFFER_POOL_VOLUME_LOCK_DECL_HPP_INCLUDED

#include <flakelib/buffer/volume_view.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/buffer_pool/volume_lock_fwd.hpp>
#include <sge/opencl/memory_object/dim3.hpp>
#include <fcppt/nonassignable.hpp>


namespace flakelib
{
namespace buffer_pool
{
template<typename T>
class volume_lock
{
FCPPT_NONASSIGNABLE(
	volume_lock);
public:
	volume_lock(
		buffer_pool::object &,
		sge::opencl::memory_object::dim3 const &);

	buffer::volume_view<T> const &
	value() const;

	~volume_lock();
private:
	buffer_pool::object &pool_;
	buffer::volume_view<T> const value_;
};
}
}

#endif
