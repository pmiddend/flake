#ifndef FLAKELIB_BUFFER_POOL_PLANAR_LOCK_DECL_HPP_INCLUDED
#define FLAKELIB_BUFFER_POOL_PLANAR_LOCK_DECL_HPP_INCLUDED

#include <flakelib/buffer_pool/planar_lock_fwd.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/buffer/planar_view.hpp>
#include <sge/opencl/memory_object/dim2.hpp>
#include <fcppt/nonassignable.hpp>

namespace flakelib
{
namespace buffer_pool
{
template<typename T>
class planar_lock
{
FCPPT_NONASSIGNABLE(
	planar_lock);
public:
	explicit
	planar_lock(
		buffer_pool::object &,
		sge::opencl::memory_object::dim2 const &);

	buffer::planar_view<T> const &
	value() const;

	~planar_lock();
private:
	buffer_pool::object &pool_;
	buffer::planar_view<T> const value_;
};
}
}

#endif
