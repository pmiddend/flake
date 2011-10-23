#include <flakelib/planar_cache.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assert/pre_message.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/container/ptr/push_back_unique_ptr.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/dim/comparison.hpp>


flakelib::planar_cache::planar_cache(
	sge::opencl::context::object &_context,
	cl_image_format const _image_format)
:
	context_(
		_context),
	image_format_(
		_image_format),
	image_pool_(),
	locked_textures_()
{
}

void
flakelib::planar_cache::lock(
	sge::opencl::memory_object::image::planar &_image)
{
	FCPPT_ASSERT_PRE(
		locked_textures_.find(&_image) == locked_textures_.end());

	locked_textures_.insert(
		&_image);
}

sge::opencl::memory_object::image::planar &
flakelib::planar_cache::get(
	sge::opencl::memory_object::size_type const edge_size)
{
	return
		this->get(
			sge::opencl::memory_object::dim2(
				edge_size,
				edge_size));
}

sge::opencl::memory_object::image::planar &
flakelib::planar_cache::get(
	sge::opencl::memory_object::dim2 const &_size)
{
	for(
		image_pool::iterator it =
		 image_pool_.begin();
		it != image_pool_.end();
		++it)
	{
		if(locked_textures_.find(&(*it)) != locked_textures_.end())
			continue;

		if(it->size() != _size)
			continue;

		// We've found a suitable, unlocked texture. Nice. :)
		return
			*it;
	}

	// We didn't find a suitable, unlocked texture. So create one
	fcppt::container::ptr::push_back_unique_ptr(
		image_pool_,
		fcppt::make_unique_ptr<sge::opencl::memory_object::image::planar>(
			fcppt::ref(
				context_),
			sge::opencl::memory_object::flags_field(
				sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
			image_format_,
			_size,
			sge::opencl::memory_object::image::planar_pitch(
				0)));

	// and return
	return
		image_pool_.back();
}

void
flakelib::planar_cache::unlock(
	sge::opencl::memory_object::image::planar &_image)
{
	locked_textures::size_type const deleted_elements = locked_textures_.erase(
		&_image);

	FCPPT_ASSERT_PRE(
		deleted_elements);
}

flakelib::planar_cache::~planar_cache()
{
}
