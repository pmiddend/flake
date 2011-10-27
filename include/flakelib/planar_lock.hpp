#ifndef FLAKELIB_PLANAR_LOCK_HPP_INCLUDED
#define FLAKELIB_PLANAR_LOCK_HPP_INCLUDED

#include <flakelib/planar_cache_fwd.hpp>
#include <sge/opencl/memory_object/dim2.hpp>
#include <sge/opencl/memory_object/size_type.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
class planar_lock
{
FCPPT_NONCOPYABLE(
	planar_lock);
public:
	explicit
	planar_lock(
		flakelib::planar_cache &,
		sge::opencl::memory_object::size_type);

	explicit
	planar_lock(
		flakelib::planar_cache &,
		sge::opencl::memory_object::dim2 const &);

	sge::opencl::memory_object::image::planar &
	value() const;

	~planar_lock();
private:
	flakelib::planar_cache &planar_cache_;
	sge::opencl::memory_object::image::planar &value_;
};
}

#endif
