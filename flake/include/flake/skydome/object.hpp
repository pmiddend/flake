#ifndef FLAKE_SKYDOME_OBJECT_HPP_INCLUDED
#define FLAKE_SKYDOME_OBJECT_HPP_INCLUDED

#include <flake/detail/symbol.hpp>
#include <flake/skydome/latitude.hpp>
#include <flake/skydome/longitude.hpp>
#include <flake/skydome/texture_path.hpp>
#include <flake/skydome/y_translation.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/context/core_fwd.hpp>
#include <sge/renderer/device/core_fwd.hpp>
#include <sge/renderer/index/buffer_unique_ptr.hpp>
#include <sge/renderer/state/core/sampler/object_unique_ptr.hpp>
#include <sge/renderer/texture/planar_unique_ptr.hpp>
#include <sge/renderer/vertex/buffer_unique_ptr.hpp>
#include <sge/renderer/vertex/declaration_unique_ptr.hpp>
#include <sge/shader/context_fwd.hpp>
#include <sge/shader/pair.hpp>
#include <sge/shader/parameter/matrix.hpp>
#include <sge/shader/parameter/planar_texture.hpp>
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
	FLAKE_DETAIL_SYMBOL
	object(
		sge::renderer::device::core &,
		sge::shader::context &,
		sge::image2d::system &_image_system,
		flake::skydome::texture_path const &,
		sge::camera::base &,
		flake::skydome::longitude const &,
		flake::skydome::latitude const &,
		flake::skydome::y_translation const &);

	FLAKE_DETAIL_SYMBOL
	void
	render(
		sge::renderer::context::core &);

	FLAKE_DETAIL_SYMBOL
	~object();
private:
	flake::skydome::y_translation const y_translation_;
	sge::renderer::device::core &renderer_;
	sge::camera::base &camera_;
	sge::renderer::vertex::declaration_unique_ptr const vertex_declaration_;
	sge::renderer::vertex::buffer_unique_ptr const vertex_buffer_;
	sge::renderer::index::buffer_unique_ptr const index_buffer_;
	sge::renderer::texture::planar_unique_ptr const texture_;
	sge::shader::pair shader_;
	sge::shader::parameter::matrix<sge::renderer::scalar,4,4> mvp_parameter_;
	sge::shader::parameter::planar_texture texture_parameter_;
	sge::renderer::state::core::sampler::object_unique_ptr const texture_state_;
};
}
}

#endif
