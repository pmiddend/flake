#ifndef FLAKE_SKYDOME_OBJECT_HPP_INCLUDED
#define FLAKE_SKYDOME_OBJECT_HPP_INCLUDED

#include <flake/shader/context_fwd.hpp>
#include <flake/shader/parameter/matrix.hpp>
#include <flake/shader/pair.hpp>
#include <flake/skydome/longitude.hpp>
#include <flake/skydome/latitude.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/renderer/vertex_buffer_scoped_ptr.hpp>
#include <sge/renderer/index_buffer_scoped_ptr.hpp>
#include <sge/renderer/context/object_fwd.hpp>
#include <fcppt/noncopyable.hpp>

namespace flake
{
namespace skydome
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	object(
		sge::renderer::device &,
		flake::shader::context &,
		sge::image2d::system &_image_system,
		sge::camera::base &,
		flake::skydome::longitude const &,
		flake::skydome::latitude const &);

	void
	render(
		sge::renderer::context::object &);

	~object();
private:
	sge::renderer::device &renderer_;
	sge::camera::base &camera_;
	sge::renderer::vertex_declaration_scoped_ptr vertex_declaration_;
	sge::renderer::vertex_buffer_scoped_ptr vertex_buffer_;
	sge::renderer::index_buffer_scoped_ptr index_buffer_;
	flake::shader::pair shader_;
	flake::shader::parameter::matrix<sge::renderer::scalar,4,4> mvp_parameter_;
};
}
}

#endif
