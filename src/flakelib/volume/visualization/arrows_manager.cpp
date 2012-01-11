#include <sge/shader/object_parameters.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <fcppt/text.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <sge/renderer/vector3.hpp>
#include <sge/shader/matrix.hpp>
#include <fcppt/assign/make_container.hpp>
#include <flakelib/volume/visualization/arrow_vf/format.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <flakelib/volume/visualization/arrows_manager.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/device.hpp>

flakelib::volume::visualization::arrows_manager::arrows_manager(
	sge::renderer::device &_renderer)
:
	renderer_(
		_renderer),
	vertex_declaration_(
		renderer_.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<arrow_vf::format>())),
	shader_(
		sge::shader::object_parameters(
			renderer_,
			*vertex_declaration_,
			sge::shader::vf_to_string<arrow_vf::format>(),
			fcppt::assign::make_container<sge::shader::variable_sequence>
				(sge::shader::variable(
					"mvp",
					sge::shader::variable_type::uniform,
					sge::shader::matrix(
						sge::renderer::matrix4::identity(),
						sge::shader::matrix_flags::projection))),
			sge::shader::sampler_sequence())
			.vertex_shader(
				flakelib::media_path_from_string(
					FCPPT_TEXT("shaders/volume_arrow/vertex.glsl")))
			.fragment_shader(
				flakelib::media_path_from_string(
					FCPPT_TEXT("shaders/volume_arrow/fragment.glsl")))
			.name(
				FCPPT_TEXT("Arrows shader")))
{
}

sge::renderer::vertex_declaration const &
flakelib::volume::visualization::arrows_manager::vertex_declaration() const
{
	return *vertex_declaration_;
}

sge::shader::object &
flakelib::volume::visualization::arrows_manager::shader()
{
	return shader_;
}

sge::shader::object const &
flakelib::volume::visualization::arrows_manager::shader() const
{
	return shader_;
}

sge::renderer::device &
flakelib::volume::visualization::arrows_manager::renderer() const
{
	return renderer_;
}

flakelib::volume::visualization::arrows_manager::~arrows_manager()
{
}
