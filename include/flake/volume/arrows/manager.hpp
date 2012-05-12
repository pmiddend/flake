#ifndef FLAKE_VOLUME_ARROWS_MANAGER_HPP_INCLUDED
#define FLAKE_VOLUME_ARROWS_MANAGER_HPP_INCLUDED

#include <flake/shader/pixel_profile.hpp>
#include <flake/shader/vertex_profile.hpp>
#include <flake/volume/arrows/object.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/cg/context/object_fwd.hpp>
#include <sge/cg/parameter/named.hpp>
#include <sge/cg/profile/object_fwd.hpp>
#include <sge/cg/program/object.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/cg/loaded_program_scoped_ptr.hpp>
#include <sge/renderer/context/object_fwd.hpp>
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
		sge::cg::context::object &,
		flake::shader::vertex_profile const &,
		flake::shader::pixel_profile const &,
		sge::camera::base &);

	void
	render(
		sge::renderer::context::object &);

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
	sge::camera::base &camera_;

	sge::cg::program::object vertex_program_;
	sge::cg::program::object pixel_program_;
	sge::renderer::cg::loaded_program_scoped_ptr loaded_vertex_program_;
	sge::renderer::cg::loaded_program_scoped_ptr loaded_pixel_program_;
	sge::cg::parameter::named mvp_parameter_;
	sge::cg::parameter::named camera_position_parameter_;

	child_sequence children_;

	void
	add_child(
		arrows::object &);
};
}
}
}

#endif

