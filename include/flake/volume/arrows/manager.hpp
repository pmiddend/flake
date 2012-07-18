#ifndef FLAKE_VOLUME_ARROWS_MANAGER_HPP_INCLUDED
#define FLAKE_VOLUME_ARROWS_MANAGER_HPP_INCLUDED

#include <flake/shader/context_fwd.hpp>
#include <flake/shader/pair.hpp>
#include <flake/shader/parameter/matrix.hpp>
#include <flake/shader/parameter/vector.hpp>
#include <flake/volume/arrows/object.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/scalar.hpp>
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
		flake::shader::context &,
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
	flake::shader::pair shader_;
	flake::shader::parameter::matrix<sge::renderer::scalar,4u,4u> mvp_parameter_;
	flake::shader::parameter::vector<sge::renderer::scalar,3u> camera_position_parameter_;
	child_sequence children_;

	void
	add_child(
		arrows::object &);
};
}
}
}

#endif

