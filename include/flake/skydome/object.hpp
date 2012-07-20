#ifndef FLAKE_SKYDOME_OBJECT_HPP_INCLUDED
#define FLAKE_SKYDOME_OBJECT_HPP_INCLUDED

#include <sge/shader/context_fwd.hpp>
#include <sge/shader/pair.hpp>
#include <sge/shader/parameter/matrix.hpp>
#include <sge/shader/parameter/planar_texture.hpp>
#include <flake/skydome/latitude.hpp>
#include <flake/skydome/longitude.hpp>
#include <flake/skydome/texture_path.hpp>
#include <flake/skydome/y_translation.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/index_buffer_scoped_ptr.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vertex_buffer_scoped_ptr.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/context/object_fwd.hpp>
#include <sge/renderer/texture/planar_scoped_ptr.hpp>
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
		sge::shader::context &,
		sge::image2d::system &_image_system,
		flake::skydome::texture_path const &,
		sge::camera::base &,
		flake::skydome::longitude const &,
		flake::skydome::latitude const &,
		flake::skydome::y_translation const &);

	void
	render(
		sge::renderer::context::object &);

	~object();
private:
	flake::skydome::y_translation const y_translation_;
	sge::renderer::device &renderer_;
	sge::camera::base &camera_;
	sge::renderer::vertex_declaration_scoped_ptr vertex_declaration_;
	sge::renderer::vertex_buffer_scoped_ptr vertex_buffer_;
	sge::renderer::index_buffer_scoped_ptr index_buffer_;
	sge::renderer::texture::planar_scoped_ptr texture_;
	sge::shader::pair shader_;
	sge::shader::parameter::matrix<sge::renderer::scalar,4,4> mvp_parameter_;
	sge::shader::parameter::planar_texture texture_parameter_;
};
}
}

#endif
