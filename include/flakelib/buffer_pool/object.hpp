#ifndef FLAKELIB_BUFFER_POOL_OBJECT_HPP_INCLUDED
#define FLAKELIB_BUFFER_POOL_OBJECT_HPP_INCLUDED

#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/memory_object/byte_size.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <set>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
namespace buffer_pool
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::opencl::context::object &);

	sge::opencl::memory_object::buffer &
	get_and_lock(
		sge::opencl::memory_object::byte_size const &);

	void
	unlock(
		sge::opencl::memory_object::buffer &);

	~object();
private:
	typedef
	boost::ptr_vector<sge::opencl::memory_object::buffer>
	pool_container;

	typedef
	std::set<sge::opencl::memory_object::buffer *>
	locked_buffers;

	sge::opencl::context::object &context_;
	pool_container pool_;
	locked_buffers locked_buffers_;
};
}
}

#endif
