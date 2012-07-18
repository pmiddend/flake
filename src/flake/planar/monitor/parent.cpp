#include <flake/media_path_from_string.hpp>
#include <flake/planar/monitor/parent.hpp>
#include <flake/planar/monitor/arrow_vf/format.hpp>
#include <sge/cg/parameter/named.hpp>
#include <sge/cg/parameter/vector/set.hpp>
#include <sge/cg/program/from_file_parameters.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/scoped_transform.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/cg/loaded_program.hpp>
#include <sge/renderer/texture/stage.hpp>
#include <sge/renderer/texture/filter/point.hpp>
#include <sge/renderer/texture/filter/scoped.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/sprite/buffers/single.hpp>
#include <sge/sprite/buffers/with_declaration.hpp>
#include <sge/sprite/compare/default.hpp>
#include <sge/sprite/intrusive/process/ordered.hpp>
#include <sge/sprite/process/all.hpp>
#include <sge/sprite/process/default_geometry_options.hpp>
#include <sge/sprite/process/with_options.hpp>
#include <sge/sprite/render/matrix_options.hpp>
#include <sge/sprite/render/options.hpp>
#include <sge/sprite/render/state_options.hpp>
#include <sge/sprite/render/vertex_options.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/dim/output.hpp>


flake::planar::monitor::parent::parent(
	sge::renderer::device &_renderer,
	flake::shader::context &_shader_context,
	sge::opencl::command_queue::object &_command_queue,
	sge::font::metrics_shared_ptr const _font_metrics,
	monitor::font_color const &_font_color)
:
	renderer_(
		_renderer),
	command_queue_(
		_command_queue),
	font_metrics_(
		_font_metrics),
	font_drawer_(
		renderer_,
		_font_color.get(),
		sge::font::text::set_matrices(
			false)),
	vd_(
		renderer_.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<arrow_vf::format>())),
	arrow_shader_(
		_shader_context,
		*vd_,
		flake::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/arrow.cg"))),
		flake::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/arrow.cg")))),
	arrow_initial_position_parameter_(
		arrow_shader_.vertex_program(),
		flake::shader::parameter::name(
			sge::cg::string(
				"initial_position")),
		sge::renderer::vector2()),
	arrow_projection_parameter_(
		arrow_shader_.vertex_program(),
		flake::shader::parameter::name(
			sge::cg::string(
				"projection")),
		flake::shader::parameter::is_projection_matrix(
			true),
		sge::renderer::matrix4()),
	sprite_system_(
		renderer_,
		sge::sprite::buffers::option::dynamic),
	sprite_collection_(),
	children_()
{
}

sge::renderer::vertex_declaration const &
flake::planar::monitor::parent::arrow_vertex_declaration() const
{
	return *vd_;
}

sge::opencl::context::object &
flake::planar::monitor::parent::cl_context() const
{
	return command_queue_.context();
}

flake::shader::pair &
flake::planar::monitor::parent::arrow_shader()
{
	return
		arrow_shader_;
}

void
flake::planar::monitor::parent::arrow_projection(
	sge::renderer::matrix4 const &_matrix)
{
	arrow_projection_parameter_.set(
		_matrix);
}

void
flake::planar::monitor::parent::arrow_position(
	sge::renderer::vector2 const &_position)
{
	arrow_initial_position_parameter_.set(
		_position);
}

sge::renderer::device &
flake::planar::monitor::parent::renderer() const
{
	return renderer_;
}

flake::planar::monitor::dummy_sprite::collection &
flake::planar::monitor::parent::sprite_collection()
{
	return sprite_collection_;
}

sge::font::metrics &
flake::planar::monitor::parent::font_metrics()
{
	return *font_metrics_;
}

sge::font::text::drawer_3d &
flake::planar::monitor::parent::font_drawer()
{
	return font_drawer_;
}

void
flake::planar::monitor::parent::render(
	sge::renderer::context::object &_context,
	monitor::optional_projection const &_projection)
{
	sge::renderer::texture::filter::scoped scoped_texture_filter(
		_context,
		sge::renderer::texture::stage(
			0u),
		sge::renderer::texture::filter::point());

	fcppt::scoped_ptr<sge::renderer::scoped_transform> projection_transform;
	fcppt::scoped_ptr<sge::renderer::scoped_transform> world_transform;

	if(_projection)
	{
		projection_transform.take(
			fcppt::make_unique_ptr<sge::renderer::scoped_transform>(
				fcppt::ref(
					_context),
				sge::renderer::matrix_mode::projection,
				*_projection));

		world_transform.take(
			fcppt::make_unique_ptr<sge::renderer::scoped_transform>(
				fcppt::ref(
					_context),
				sge::renderer::matrix_mode::world,
				sge::renderer::matrix4::identity()));

		sge::sprite::process::with_options
		<
			sge::sprite::process::options
			<
				sge::sprite::process::default_geometry_options
				<
					dummy_sprite::choices,
					sge::sprite::compare::default_
				>::value,
				sge::sprite::render::options
				<
					sge::sprite::render::matrix_options::nothing,
					sge::sprite::render::state_options::set,
					sge::sprite::render::vertex_options::declaration_and_buffer
				>
			>
		>(
			_context,
			sprite_collection_.range(),
			sprite_system_,
			sge::sprite::compare::default_());
	}
	else
	{
		sge::sprite::process::all(
			_context,
			sprite_collection_.range(),
			sprite_system_,
			sge::sprite::compare::default_());
	}

	for(child_list::iterator it = children_.begin(); it != children_.end(); ++it)
		it->render(
			_context,
			_projection);
}

void
flake::planar::monitor::parent::update()
{
	for(child_list::iterator it = children_.begin(); it != children_.end(); ++it)
		it->update();
}

flake::planar::monitor::parent::~parent()
{
}

void
flake::planar::monitor::parent::add_child(
	monitor::child &_child)
{
	children_.push_back(
		_child);
}

void
flake::planar::monitor::parent::erase_child(
	monitor::child &_child)
{
	child_list::iterator it;
	for(
		it = children_.begin();
		it != children_.end() && &(*it) != &_child;
		++it) ;

	FCPPT_ASSERT_PRE(
		it != children_.end());

	children_.erase(
		it);
}
