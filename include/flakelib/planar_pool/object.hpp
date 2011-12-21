#ifndef FLAKELIB_PLANAR_POOL_HPP_INCLUDED
#define FLAKELIB_PLANAR_POOL_HPP_INCLUDED

#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/memory_object/dim2.hpp>
#include <flakelib/planar_buffer_fwd.hpp>
#include <flakelib/buffer_element_stride.hpp>
#include <flakelib/planar_pool/unique_lock.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/unique_ptr.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <set>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace planar_pool
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::opencl::context::object &);

	planar_pool::unique_lock
	get_planar(
		sge::opencl::memory_object::dim2 const &,
		flakelib::buffer_element_stride const &);

	~object();
private:
	friend class planar_pool::scoped_lock;

	typedef
	boost::ptr_vector<sge::opencl::memory_object::buffer>
	image_pool;

	typedef
	std::set<sge::opencl::memory_object::buffer *>
	locked_buffers;

	sge::opencl::context::object &context_;
	image_pool image_pool_;
	locked_buffers locked_buffers_;

	void
	unlock(
		sge::opencl::memory_object::buffer &);
};
}
}

#endif
