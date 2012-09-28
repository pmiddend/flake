#ifndef FLAKE_VOLUME_ARROWS_MANAGER_HPP_INCLUDED
#define FLAKE_VOLUME_ARROWS_MANAGER_HPP_INCLUDED

#include <flake/volume/arrows/object.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/cg/loaded_program_scoped_ptr.hpp>
#include <sge/renderer/context/core_fwd.hpp>
#include <sge/renderer/device/core_fwd.hpp>
#include <sge/renderer/state/core/blend/object_scoped_ptr.hpp>
#include <sge/shader/context_fwd.hpp>
#include <sge/shader/pair.hpp>
#include <sge/shader/parameter/matrix.hpp>
#include <sge/shader/parameter/vector.hpp>
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
		sge::renderer::device::core &,
		sge::shader::context &,
		sge::camera::base &);

	void
	render(
		sge::renderer::context::core &);

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

	sge::renderer::device::core &renderer_;
	sge::renderer::vertex_declaration_scoped_ptr const vertex_declaration_;
	sge::renderer::state::core::blend::object_scoped_ptr const blend_state_;
	sge::camera::base &camera_;
	sge::shader::pair shader_;
	sge::shader::parameter::matrix<sge::renderer::scalar,4u,4u> mvp_parameter_;
	sge::shader::parameter::vector<sge::renderer::scalar,3u> camera_position_parameter_;
	child_sequence children_;

	void
	add_child(
		arrows::object &);
};
}
}
}

#endif

