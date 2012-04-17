#ifndef FLAKE_FONT_METRICS_CACHE_HPP_INCLUDED
#define FLAKE_FONT_METRICS_CACHE_HPP_INCLUDED

#include <sge/font/metrics_fwd.hpp>
#include <sge/font/size_type.hpp>
#include <sge/font/system_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <utility>
#include <fcppt/config/external_end.hpp>


namespace flake
{
class font_metrics_cache
{
FCPPT_NONCOPYABLE(
	font_metrics_cache);
public:
	explicit
	font_metrics_cache(
		sge::font::system &);

	sge::font::metrics &
	get(
		boost::filesystem::path const &,
		sge::font::size_type);

	~font_metrics_cache();
private:
	typedef
	std::pair<boost::filesystem::path,sge::font::size_type>
	font_descriptor;

	typedef
	boost::ptr_map
	<
		font_descriptor,
		sge::font::metrics
	>
	descriptor_to_metrics_map;

	sge::font::system &font_system_;
	descriptor_to_metrics_map descriptor_to_metrics_;
};
}

#endif

