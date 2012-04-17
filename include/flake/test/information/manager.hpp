#ifndef FLAKE_TEST_INFORMATION_MANAGER_HPP_INCLUDED
#define FLAKE_TEST_INFORMATION_MANAGER_HPP_INCLUDED

#include <flake/test/information/object.hpp>
#include <sge/font/metrics_fwd.hpp>
#include <sge/font/text/drawer_3d.hpp>
#include <sge/image/color/any/object_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace test
{
namespace information
{
class manager
{
FCPPT_NONCOPYABLE(
	manager);
public:
	explicit
	manager(
		sge::font::metrics &,
		sge::renderer::device &,
		sge::image::color::any::object const &);

	void
	render();

	~manager();
private:
	friend class information::object;

	typedef
	boost::intrusive::list
	<
		information::object,
		boost::intrusive::constant_time_size<false>
	>
	child_sequence;

	sge::renderer::device &renderer_;
	sge::font::metrics &font_metrics_;
	sge::font::text::drawer_3d font_drawer_;
	child_sequence children_;

	void
	add_child(
		information::object &);
};
}
}
}

#endif

