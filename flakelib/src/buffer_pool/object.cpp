#include <flakelib/buffer_pool/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>

flakelib::buffer_pool::object::object(
	sge::opencl::context::object &_context)
:
	context_(
		_context),
	pool_(),
	locked_buffers_(),
	memory_consumption_(
		0u)
{
}

sge::opencl::memory_object::buffer &
flakelib::buffer_pool::object::get_and_lock(
	sge::opencl::memory_object::byte_size const &_byte_size)
{
	for(
		auto &buffer
		:
		pool_
	)
	{
		// Buffer is locked?
		if(locked_buffers_.find(buffer.get_pointer()) != locked_buffers_.end())
			continue;

		// Buffer isn't the right size?
		if(buffer->byte_size() != _byte_size)
			continue;

		// We found one (that was already created), so put it in
		// locked_buffers and return it.
		locked_buffers_.insert(
			buffer.get_pointer());

		return
			*buffer;
	}

	// We didn't find a suitable, unlocked texture. So create one
	pool_.push_back(
		fcppt::make_unique_ptr<sge::opencl::memory_object::buffer>(
			context_,
			sge::opencl::memory_object::flags_field{
				sge::opencl::memory_object::flags::read,
				sge::opencl::memory_object::flags::write},
			_byte_size));

	locked_buffers_.insert(
		pool_.back().get_pointer());

	return
		*pool_.back();
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

sge::opencl::memory_object::byte_size const
flakelib::buffer_pool::object::memory_consumption() const
{
	sge::opencl::memory_object::byte_size result(
		0u);

	for(
		auto const &buffer
		:
		pool_
	)
		result += buffer->byte_size();
	return
		result;
}

flakelib::buffer_pool::object::~object()
{
}
