#ifndef FLAKE_VOLUME_ARROWS_MANAGER_HPP_INCLUDED
#define FLAKE_VOLUME_ARROWS_MANAGER_HPP_INCLUDED

#include <flake/volume/arrows/object.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/shader/object.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace volume
{
namespace arrows
{
class manager
{
FCPPT_NONCOPYABLE(
	manager);
public:
	explicit
	manager(
		sge::renderer::device &,
		sge::camera::base &);

	void
	render();

	~manager();
private:
	friend class arrows::object;

	typedef
	boost::intrusive::list
	<
		arrows::object,
		boost::intrusive::constant_time_size<false>
	>
	child_sequence;

	sge::renderer::device &renderer_;
	sge::renderer::vertex_declaration_scoped_ptr vertex_declaration_;
	sge::shader::object shader_;
	sge::camera::base &camera_;
	child_sequence children_;

	void
	add_child(
		arrows::object &);
};
}
}
}

#endif

