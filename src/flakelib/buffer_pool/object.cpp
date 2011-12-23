#include <flakelib/buffer_pool/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/container/ptr/push_back_unique_ptr.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>

flakelib::buffer_pool::object::object(
	sge::opencl::context::object &_context)
:
	context_(
		_context),
	pool_(),
	locked_buffers_()
{
}

sge::opencl::memory_object::buffer &
flakelib::buffer_pool::object::get_and_lock(
	sge::opencl::memory_object::byte_size const &_byte_size)
{
	for(
		pool_container::iterator it =
			pool_.begin();
		it != pool_.end();
		++it)
	{
		// Buffer is locked?
		if(locked_buffers_.find(&(*it)) != locked_buffers_.end())
			continue;

		// Buffer isn't the right size?
		if(it->byte_size() != _byte_size.get())
			continue;

		// We found one (that was already created), so put it in
		// locked_buffers and return it.
		locked_buffers_.insert(
			&(*it));

		return
			*it;
	}

	// We didn't find a suitable, unlocked texture. So create one
	fcppt::container::ptr::push_back_unique_ptr(
		pool_,
		fcppt::make_unique_ptr<sge::opencl::memory_object::buffer>(
			fcppt::ref(
				context_),
			sge::opencl::memory_object::flags_field(
				sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
			_byte_size));

	locked_buffers_.insert(
		&pool_.back());

	return
		pool_.back();
}

void
flakelib::buffer_pool::object::unlock(
	sge::opencl::memory_object::buffer &_buffer)
{
	locked_buffers::size_type const deleted_elements =
		locked_buffers_.erase(
			&_buffer);

	FCPPT_ASSERT_PRE(
		deleted_elements);
}

flakelib::buffer_pool::object::~object()
{
}
