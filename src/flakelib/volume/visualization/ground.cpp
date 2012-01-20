#include <sge/renderer/texture/filter/linear.hpp>
#include <sge/renderer/texture/filter/trilinear.hpp>
#include <sge/renderer/texture/filter/scoped.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/image2d/view/const_elements_wrapper.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <sge/renderer/texture/create_planar_from_view.hpp>
#include <sge/renderer/vf/vertex.hpp>
#include <sge/renderer/vf/iterator.hpp>
#include <flakelib/volume/visualization/model_vf/format_part_view.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/scoped.hpp>
#include <flakelib/volume/visualization/model_vf/format.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <fcppt/assign/make_container.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/renderer/texture/mipmap/all_levels.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <fcppt/text.hpp>
#include <flakelib/volume/visualization/ground.hpp>
#include <sge/renderer/device.hpp>
#include <sge/image2d/system.hpp>
#include <sge/image2d/file.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>

flakelib::volume::visualization::ground::ground(
	sge::renderer::device &_renderer,
	sge::renderer::vertex_declaration const &_vertex_declaration,
	sge::image2d::system &_image_system,
	visualization::grid_size const &_grid_size)
:
	renderer_(
		_renderer),
	vertex_declaration_(
		_vertex_declaration),
	shader_(
		sge::shader::object_parameters(
			renderer_,
			vertex_declaration_,
			sge::shader::vf_to_string<model_vf::format>(),
			fcppt::assign::make_container<sge::shader::variable_sequence>
				(sge::shader::variable(
					"mvp",
					sge::shader::variable_type::uniform,
					sge::shader::matrix(
						sge::renderer::matrix4::identity(),
						sge::shader::matrix_flags::projection))),
			fcppt::assign::make_container<sge::shader::sampler_sequence>
				(sge::shader::sampler(
					"ground_texture",
					sge::renderer::texture::create_planar_from_view(
						renderer_,
						_image_system.load(
							flakelib::media_path_from_string(
								FCPPT_TEXT("textures/ground.png")))->view(),
						// FIXME: Mipmaps advisable here?
						sge::renderer::texture::mipmap::all_levels(
							sge::renderer::texture::mipmap::auto_generate::yes),
						sge::renderer::resource_flags::none)))
				(sge::shader::sampler(
					"snow_distribution",
					sge::renderer::texture::planar_ptr())))
				.name(
					FCPPT_TEXT("ground shader"))
				.vertex_shader(
					flakelib::media_path_from_string(
						FCPPT_TEXT("shaders/ground/vertex.glsl")))
				.fragment_shader(
					flakelib::media_path_from_string(
						FCPPT_TEXT("shaders/ground/fragment.glsl")))),
	vertex_buffer_(
		_renderer.create_vertex_buffer(
			vertex_declaration_,
			sge::renderer::vf::dynamic::part_index(
				0u),
			sge::renderer::vertex_count(
				static_cast<sge::renderer::size_type>(
					6)),
			sge::renderer::resource_flags::none))
{
	sge::renderer::scoped_vertex_lock const vblock(
		*vertex_buffer_,
		sge::renderer::lock_mode::writeonly);

	model_vf::format_part_view const vertices(
		vblock.value());

	model_vf::format_part_view::iterator vb_it(
		vertices.begin());

	vb_it->set<model_vf::position>(
		model_vf::position::packed_type(
			0.0f,
			0.0f,
			0.0f));

	(vb_it++)->set<model_vf::texcoord>(
		model_vf::texcoord::packed_type(
			0.0f,
			0.0f));

	vb_it->set<model_vf::position>(
		model_vf::position::packed_type(
			static_cast<sge::renderer::scalar>(
				_grid_size.get()[0]),
			0.0f,
			0.0f));

	(vb_it++)->set<model_vf::texcoord>(
		model_vf::texcoord::packed_type(
			1.0f,
			0.0f));

	vb_it->set<model_vf::position>(
		model_vf::position::packed_type(
			static_cast<sge::renderer::scalar>(
				_grid_size.get()[0]),
			0.0f,
			static_cast<sge::renderer::scalar>(
				_grid_size.get()[2])));

	(vb_it++)->set<model_vf::texcoord>(
		model_vf::texcoord::packed_type(
			1.0f,
			1.0f));

	vb_it->set<model_vf::position>(
		model_vf::position::packed_type(
			static_cast<sge::renderer::scalar>(
				_grid_size.get()[0]),
			0.0f,
			static_cast<sge::renderer::scalar>(
				_grid_size.get()[2])));

	(vb_it++)->set<model_vf::texcoord>(
		model_vf::texcoord::packed_type(
			1.0f,
			1.0f));

	vb_it->set<model_vf::position>(
		model_vf::position::packed_type(
			0.0f,
			0.0f,
			static_cast<sge::renderer::scalar>(
				_grid_size.get()[2])));

	(vb_it++)->set<model_vf::texcoord>(
		model_vf::texcoord::packed_type(
			0.0f,
			1.0f));

	vb_it->set<model_vf::position>(
		model_vf::position::packed_type(
			0.0f,
			0.0f,
			0.0f));

	(vb_it++)->set<model_vf::texcoord>(
		model_vf::texcoord::packed_type(
			0.0f,
			0.0f));
}

void
flakelib::volume::visualization::ground::render(
	sge::renderer::matrix4 const &_mvp,
	sge::renderer::texture::planar_ptr const _snow_distribution)
{
	shader_.update_texture(
		"snow_distribution",
		_snow_distribution);

	sge::renderer::texture::filter::scoped scoped_texture_filter(
		renderer_,
		sge::renderer::texture::stage(
			0),
		sge::renderer::texture::filter::trilinear());

	sge::renderer::texture::filter::scoped scoped_texture_filter2(
		renderer_,
		sge::renderer::texture::stage(
			1),
		sge::renderer::texture::filter::linear());

	sge::shader::scoped scoped_shader(
		shader_,
		sge::shader::activate_everything());

	shader_.update_uniform(
		"mvp",
		sge::shader::matrix(
			_mvp,
			sge::shader::matrix_flags::projection));

	sge::renderer::scoped_vertex_buffer scoped_vb(
		renderer_,
		*vertex_buffer_);

	renderer_.render_nonindexed(
		sge::renderer::first_vertex(
			0),
		sge::renderer::vertex_count(
			vertex_buffer_->size()),
		sge::renderer::nonindexed_primitive_type::triangle);
}

flakelib::volume::visualization::ground::~ground()
{
}
