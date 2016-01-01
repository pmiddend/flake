#include <flake/media_path_from_string.hpp>
#include <flake/planar/monitor/parent.hpp>
#include <flake/planar/monitor/arrow_vf/format.hpp>
#include <flake/planar/monitor/dummy_sprite/state_choices.hpp>
#include <flake/planar/monitor/dummy_sprite/state_options.hpp>
#include <flake/planar/monitor/dummy_sprite/state_parameters.hpp>
#include <sge/cg/parameter/named.hpp>
#include <sge/cg/parameter/vector/set.hpp>
#include <sge/cg/program/from_file_parameters.hpp>
#include <sge/font/object.hpp>
#include <sge/font/parameters.hpp>
#include <sge/font/system.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/renderer/cg/loaded_program.hpp>
#include <sge/renderer/context/ffp.hpp>
#include <sge/renderer/device/ffp.hpp>
#include <sge/renderer/state/core/sampler/object.hpp>
#include <sge/renderer/state/core/sampler/parameters.hpp>
#include <sge/renderer/state/core/sampler/scoped.hpp>
#include <sge/renderer/state/core/sampler/address/default.hpp>
#include <sge/renderer/state/core/sampler/address/parameters.hpp>
#include <sge/renderer/state/core/sampler/filter/point.hpp>
#include <sge/renderer/state/ffp/transform/object.hpp>
#include <sge/renderer/state/ffp/transform/object_unique_ptr.hpp>
#include <sge/renderer/state/ffp/transform/parameters.hpp>
#include <sge/renderer/state/ffp/transform/scoped.hpp>
#include <sge/renderer/vertex/declaration.hpp>
#include <sge/renderer/vertex/declaration_parameters.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/sprite/buffers/single.hpp>
#include <sge/sprite/buffers/with_declaration.hpp>
#include <sge/sprite/compare/default.hpp>
#include <sge/sprite/intrusive/process/ordered.hpp>
#include <sge/sprite/process/all.hpp>
#include <sge/sprite/process/default_geometry_options.hpp>
#include <sge/sprite/process/with_options.hpp>
#include <fcppt/make_cref.hpp>
#include <fcppt/optional/maybe.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_map.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/math/matrix/identity.hpp>
#include <fcppt/math/vector/null.hpp>


flake::planar::monitor::parent::parent(
	sge::renderer::device::ffp &_renderer,
	sge::shader::context &_shader_context,
	sge::opencl::command_queue::object &_command_queue,
	sge::font::system &_font_system,
	sge::font::ttf_size const _ttf_size,
	monitor::font_color const &_font_color)
:
	renderer_(
		_renderer),
	command_queue_(
		_command_queue),
	font_system_(
		_font_system),
	font_color_(
		_font_color),
	font_(
		font_system_.create_font(
			sge::font::parameters()
				.family(
					FCPPT_TEXT("sans"))
				.ttf_size(
					_ttf_size))),
	vd_(
		renderer_.create_vertex_declaration(
			sge::renderer::vertex::declaration_parameters(
				sge::renderer::vf::dynamic::make_format<arrow_vf::format>()))),
	point_sampler_(
		renderer_.create_sampler_state(
			sge::renderer::state::core::sampler::parameters(
				sge::renderer::state::core::sampler::address::default_(),
				sge::renderer::state::core::sampler::filter::point()))),
	arrow_shader_(
		_shader_context,
		*vd_,
		sge::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/arrow.cg"))),
		sge::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/arrow.cg"))),
		sge::shader::optional_cflags()),
	arrow_initial_position_parameter_(
		arrow_shader_.vertex_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"initial_position")),
		fcppt::math::vector::null<
			sge::renderer::vector2
		>()
	),
	arrow_projection_parameter_(
		arrow_shader_.vertex_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"projection")),
		renderer_,
		sge::shader::parameter::is_projection_matrix(
			true),
		fcppt::math::matrix::identity<
			sge::renderer::matrix4
		>()
	),
	sprite_buffers_(
		renderer_,
		sge::sprite::buffers::option::dynamic),
	sprite_states_(
		renderer_,
		flake::planar::monitor::dummy_sprite::state_parameters()),
	sprite_collection_(),
	children_()
{
}

sge::renderer::vertex::declaration const &
flake::planar::monitor::parent::arrow_vertex_declaration() const
{
	return *vd_;
}

sge::opencl::context::object &
flake::planar::monitor::parent::cl_context() const
{
	return command_queue_.context();
}

sge::shader::pair &
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

flake::planar::monitor::font_color const &
flake::planar::monitor::parent::font_color() const
{
	return
		font_color_;
}

sge::renderer::device::ffp &
flake::planar::monitor::parent::renderer() const
{
	return renderer_;
}

flake::planar::monitor::dummy_sprite::collection &
flake::planar::monitor::parent::sprite_collection()
{
	return sprite_collection_;
}

sge::font::system &
flake::planar::monitor::parent::font_system() const
{
	return font_system_;
}

sge::font::object &
flake::planar::monitor::parent::font()
{
	return *font_;
}

void
flake::planar::monitor::parent::render(
	sge::renderer::context::ffp &_context,
	monitor::optional_projection const &_opt_projection)
{
	sge::renderer::state::core::sampler::scoped const scoped_filter(
		_context,
		fcppt::assign::make_map<sge::renderer::state::core::sampler::const_object_ref_map>
		(
			sge::renderer::texture::stage(
				0u),
			fcppt::make_cref(
				*point_sampler_)));

	fcppt::optional::maybe(
		_opt_projection,
		[
			this,
			&_context
		]{
			sge::sprite::process::all(
				_context,
				sprite_collection_.range(),
				sprite_buffers_,
				sge::sprite::compare::default_(),
				sprite_states_);
		},
		[
			this,
			&_context
		](
			sge::renderer::matrix4 const &_projection
		)
		{
			sge::renderer::state::ffp::transform::object_unique_ptr const
				projection_state(
					renderer_.create_transform_state(
						sge::renderer::state::ffp::transform::parameters(
							_projection))),
				world_state(
					renderer_.create_transform_state(
						sge::renderer::state::ffp::transform::parameters(
							fcppt::math::matrix::identity<
								sge::renderer::matrix4
							>()
						)
					)
				);

			sge::renderer::state::ffp::transform::scoped const
				projection_transform(
					_context,
					sge::renderer::state::ffp::transform::mode::projection,
					*projection_state),
				world_transform(
					_context,
					sge::renderer::state::ffp::transform::mode::world,
					*world_state);

			sge::sprite::process::with_options
			<
				sge::sprite::process::options
				<
					sge::sprite::process::default_geometry_options
					<
						flake::planar::monitor::dummy_sprite::choices,
						sge::sprite::compare::default_
					>::value
				>
			>(
				_context,
				sprite_collection_.range(),
				sprite_buffers_,
				sge::sprite::compare::default_(),
				sprite_states_,
				flake::planar::monitor::dummy_sprite::state_options(
					sge::sprite::state::vertex_options::declaration_and_buffer)
					.no_transform_state());
		}
	);

	for(child_list::iterator it = children_.begin(); it != children_.end(); ++it)
		it->render(
			_context,
			_opt_projection);
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
