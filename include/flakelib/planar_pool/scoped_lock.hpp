#ifndef FLAKELIB_PLANAR_POOL_SCOPED_LOCK_HPP_INCLUDED
#define FLAKELIB_PLANAR_POOL_SCOPED_LOCK_HPP_INCLUDED

#include <flakelib/planar_pool/object_fwd.hpp>
#include <sge/opencl/memory_object/dim2.hpp>
#include <sge/opencl/memory_object/size_type.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace planar_pool
{
class scoped_lock
{
FCPPT_NONCOPYABLE(
	scoped_lock);
public:
	explicit
	scoped_lock(
		planar_pool::object &,
		sge::opencl::memory_object::size_type);

	explicit
	scoped_lock(
		planar_pool::object &,
		sge::opencl::memory_object::dim2 const &);

	sge::opencl::memory_object::image::planar &
	value() const;

	~scoped_lock();
private:
	planar_pool::object &pool_;
	sge::opencl::memory_object::image::planar &value_;
};
}
}

#endif
