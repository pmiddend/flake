#ifndef FLAKE_POSTPROCESSING_CONTEXT_HPP_INCLUDED
#define FLAKE_POSTPROCESSING_CONTEXT_HPP_INCLUDED

#include <sge/renderer/context/object_unique_ptr.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/viewport/manager_fwd.hpp>
#include <sge/renderer/target/offscreen_scoped_ptr.hpp>
#include <sge/renderer/texture/planar_scoped_ptr.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <sge/cg/context/object_fwd.hpp>
#include <flake/shader/vertex_profile.hpp>
#include <flake/shader/pixel_profile.hpp>

namespace flake
{
namespace postprocessing
{
class context
{
FCPPT_NONCOPYABLE(
	context);
public:
	context(
		sge::renderer::device &,
		sge::viewport::manager &,
		sge::cg::context::object &,
		flake::shader::vertex_profile const &,
		flake::shader::pixel_profile const &);

	sge::renderer::context::object_unique_ptr
	create_render_context();

	~context();
private:
	fcppt::signal::scoped_connection viewport_connection_;
	sge::renderer::target::offscreen_scoped_ptr offscreen_target_;
	sge::renderer::texture::planar_scoped_ptr rendering_result_texture_;

	void
	viewport_callback();
};
}
}

#endif
