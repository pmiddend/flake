#ifndef FLAKE_TEST_INFORMATION_MANAGER_HPP_INCLUDED
#define FLAKE_TEST_INFORMATION_MANAGER_HPP_INCLUDED

#include <flake/test/information/object.hpp>
#include <sge/font/object_scoped_ptr.hpp>
#include <sge/font/system_fwd.hpp>
#include <sge/font/ttf_size.hpp>
#include <sge/image/color/any/object.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/context/object_fwd.hpp>
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
		sge::font::system &,
		sge::font::ttf_size,
		sge::renderer::device &,
		sge::image::color::any::object const &);

	void
	render(
		sge::renderer::context::object &);

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
	sge::image::color::any::object const color_;
	sge::font::object_scoped_ptr font_;
	child_sequence children_;

	void
	add_child(
		information::object &);
};
}
}
}

#endif

