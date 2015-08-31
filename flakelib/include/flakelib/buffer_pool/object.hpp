#ifndef FLAKELIB_BUFFER_POOL_OBJECT_HPP_INCLUDED
#define FLAKELIB_BUFFER_POOL_OBJECT_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/memory_object/byte_size.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/unique_ptr.hpp>
#include <fcppt/config/external_begin.hpp>
#include <set>
#include <vector>
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
	FLAKELIB_SYMBOL
	explicit
	object(
		sge::opencl::context::object &);

	FLAKELIB_SYMBOL
	sge::opencl::memory_object::buffer &
	get_and_lock(
		sge::opencl::memory_object::byte_size const &);

	FLAKELIB_SYMBOL
	void
	unlock(
		sge::opencl::memory_object::buffer &);

	FLAKELIB_SYMBOL
	sge::opencl::memory_object::byte_size const
	memory_consumption() const;

	FLAKELIB_SYMBOL
	~object();
private:
	typedef
	std::vector
	<
		fcppt::unique_ptr
		<
			sge::opencl::memory_object::buffer
		>
	>
	pool_container;

	typedef
	std::set<sge::opencl::memory_object::buffer *>
	locked_buffers;

	sge::opencl::context::object &context_;
	pool_container pool_;
	locked_buffers locked_buffers_;
	sge::opencl::memory_object::byte_size memory_consumption_;
};
}
}

#endif
