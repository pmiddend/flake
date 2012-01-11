#include <sge/renderer/texture/planar.hpp>
#include <sge/model/obj/vb_converter/convert.hpp>
#include <fcppt/cref.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <sge/model/obj/instance.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/first_vertex.hpp>
#include <sge/renderer/nonindexed_primitive_type.hpp>
#include <sge/renderer/resource_flags.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/renderer/texture/scoped.hpp>
#include <sge/renderer/texture/stage.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_count.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <flakelib/volume/visualization/model_vf/position.hpp>
#include <flakelib/volume/visualization/model_vf/normal.hpp>
#include <flakelib/volume/visualization/model_vf/texcoord.hpp>
#include <flakelib/volume/visualization/model_vf/format.hpp>
#include <flakelib/volume/visualization/model_vf/format_part.hpp>
#include <flakelib/volume/visualization/model_vf/format_part_view.hpp>
#include <flakelib/volume/visualization/model_vf/from_obj_map.hpp>
#include <flakelib/volume/visualization/compiled_model.hpp>

flakelib::volume::visualization::compiled_model::compiled_model(
	sge::model::obj::instance_ptr const _model,
	sge::renderer::device &_renderer,
	sge::renderer::vertex_declaration const &_vd,
	sge::renderer::texture::planar_ptr const _texture)
:
	vd_(
		_vd),
	vb_(
		sge::model::obj::vb_converter::convert
		<
			model_vf::format_part,
			model_vf::from_obj_map
		>(
			renderer_,
			vd_,
			sge::renderer::resource_flags::none,
			*_model)),
	texture_(
		_texture),
	renderer_(
		_renderer)
{
}

void
flakelib::volume::visualization::compiled_model::render()
{
	fcppt::scoped_ptr<sge::renderer::texture::scoped> scoped_texture;

	if(texture_)
		scoped_texture.take(
			fcppt::make_unique_ptr
			<
				sge::renderer::texture::scoped
			>(
				fcppt::ref(
					renderer_),
				fcppt::cref(
					*texture_),
				sge::renderer::texture::stage(
					0)));

	sge::renderer::scoped_vertex_buffer scoped_vb(
		renderer_,
		*vb_);

	renderer_.render_nonindexed(
		sge::renderer::first_vertex(
			0),
		sge::renderer::vertex_count(
			vb_->size()),
		sge::renderer::nonindexed_primitive_type::triangle);
}

flakelib::volume::visualization::compiled_model::~compiled_model()
{
}
