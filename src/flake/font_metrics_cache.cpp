#include <flake/font_metrics_cache.hpp>
#include <sge/font/system.hpp>
#include <sge/font/metrics.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/container/ptr/insert_unique_ptr_map.hpp>


flake::font_metrics_cache::font_metrics_cache(
	sge::font::system &_font_system)
:
	font_system_(
		_font_system),
	descriptor_to_metrics_()
{
}

sge::font::metrics &
flake::font_metrics_cache::get(
	boost::filesystem::path const &_path,
	sge::font::size_type const _size)
{
	font_descriptor const descriptor(
		_path,
		_size);

	descriptor_to_metrics_map::iterator it =
		descriptor_to_metrics_.find(
			descriptor);

	if(it != descriptor_to_metrics_.end())
		return *(it->second);

	return
		*fcppt::container::ptr::insert_unique_ptr_map(
			descriptor_to_metrics_,
			descriptor,
			font_system_.create_font(
				_path,
				_size)).first->second;
}

flake::font_metrics_cache::~font_metrics_cache()
{
}
