#ifndef FLAKELIB_VOLUME_VISUALIZATION_COMPILED_MODEL_HPP_INCLUDED
#define FLAKELIB_VOLUME_VISUALIZATION_COMPILED_MODEL_HPP_INCLUDED

#include <sge/model/obj/instance_ptr.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_declaration.hpp>

namespace flakelib
{
namespace volume
{
namespace visualization
{
class compiled_model
{
FCPPT_NONCOPYABLE(
	compiled_model);
public:
	explicit
	compiled_model(
		sge::model::obj::instance_ptr,
		sge::renderer::device &,
		sge::renderer::vertex_declaration const &,
		sge::renderer::texture::planar_ptr);

	void
	render();

	~compiled_model();
private:
	sge::renderer::device &renderer_;
	sge::renderer::vertex_declaration const &vd_;
	sge::renderer::vertex_buffer_ptr vb_;
	sge::renderer::texture::planar_ptr texture_;
};
}
}
}

#endif
