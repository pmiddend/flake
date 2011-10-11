#include <flakelib/visualization/monitor/planar_arrows.hpp>
#include <flakelib/visualization/monitor/parent.hpp>
#include <flakelib/visualization/monitor/dummy_sprite/parameters.hpp>
#include <sge/opencl/memory_object/renderer_buffer_lock_mode.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/matrix.hpp>
#include <sge/shader/matrix_flags.hpp>
#include <sge/shader/object.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/first_index.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/vertex_count.hpp>
#include <sge/renderer/nonindexed_primitive_type.hpp>
#include <sge/sprite/parameters.hpp>
#include <sge/texture/part_raw.hpp>
#include <sge/renderer/viewport_size.hpp>
#include <sge/renderer/projection/orthogonal_wh.hpp>
#include <sge/renderer/projection/dim.hpp>
#include <sge/renderer/projection/near.hpp>
#include <sge/renderer/projection/far.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/size_type.hpp>
#include <sge/renderer/resource_flags.hpp>
#include <sge/renderer/vf/dynamic/part_index.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/dim/arithmetic.hpp>
#include <fcppt/math/vector/structure_cast.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/text.hpp>

flakelib::visualization::monitor::planar_arrows::planar_arrows(
	monitor::parent &_parent,
	monitor::name const &_name,
	monitor::grid_dimensions const &_dimensions,
	monitor::arrow_scale const &_arrow_scale,
	monitor::grid_scale const &_grid_scale,
	sge::renderer::texture::planar_ptr const _optional_texture)
:
	monitor::child(
		_parent),
	name_(
		_name.get()),
	dimensions_(
		_dimensions.get()),
	arrow_scale_(
		_arrow_scale),
	grid_scale_(
		_grid_scale),
	position_(),
	vb_(
		child::parent().renderer().create_vertex_buffer(
			child::parent().vertex_declaration(),
			sge::renderer::vf::dynamic::part_index(
				0u),
			static_cast<sge::renderer::size_type>(
				dimensions_.content() * 2),
			sge::renderer::resource_flags::readable)),
	cl_vb_(
		child::parent().context(),
		*vb_,
		sge::opencl::memory_object::renderer_buffer_lock_mode::write_only),
	sprite_()
{
	if(_optional_texture)
		sprite_.take(
			fcppt::make_unique_ptr<dummy_sprite::object>(
				dummy_sprite::parameters()
					.size(
						fcppt::math::dim::structure_cast<dummy_sprite::object::dim>(
							this->area().size()))
					.texture(
						fcppt::make_shared_ptr<sge::texture::part_raw>(
							_optional_texture))
					.system(
						&child::parent().sprite_system())
					.elements()));
}

void
flakelib::visualization::monitor::planar_arrows::from_planar_object(
	flakelib::planar_object const &_planar_object)
{
	child::parent().to_vb(
		_planar_object,
		cl_vb_,
		grid_scale_,
		arrow_scale_);
}

void
flakelib::visualization::monitor::planar_arrows::position(
	monitor::rect::vector const &_position)
{
	position_ =
		_position;
	if(sprite_)
		sprite_->pos(
			fcppt::math::vector::structure_cast<monitor::dummy_sprite::object::vector>(
				_position));
}

flakelib::visualization::monitor::rect const
flakelib::visualization::monitor::planar_arrows::area() const
{
	return 
		monitor::rect(
			position_,
			grid_scale_.get() * 
			fcppt::math::dim::structure_cast<monitor::rect::dim>(
				dimensions_));
}

fcppt::string const
flakelib::visualization::monitor::planar_arrows::name() const
{
	return name_;
}

void
flakelib::visualization::monitor::planar_arrows::render()
{
	// Activate the shader and the vertex declaration
	sge::shader::scoped scoped_shader(
		child::parent().arrow_shader(),
		sge::shader::activate_everything());

	sge::renderer::scoped_vertex_buffer scoped_vb(
		child::parent().renderer(),
		*vb_);

	child::parent().arrow_shader().update_uniform(
		"projection",
		sge::shader::matrix(
			sge::renderer::projection::orthogonal_wh(
				fcppt::math::dim::structure_cast<sge::renderer::projection::dim>(
						sge::renderer::viewport_size(
							child::parent().renderer())),
				sge::renderer::projection::near(0.0f),
				sge::renderer::projection::far(10.0f)),
			sge::shader::matrix_flags::projection));

	child::parent().arrow_shader().update_uniform(
		"initial_position",
		position_);

	child::parent().renderer().render_nonindexed(
		sge::renderer::first_vertex(
			0),
		sge::renderer::vertex_count(
			vb_->size()),
		sge::renderer::nonindexed_primitive_type::line);
}

flakelib::visualization::monitor::planar_arrows::~planar_arrows()
{
}
