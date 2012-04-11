#include <flake/volume/arrows/manager.hpp>
#include <flake/volume/arrows/object.hpp>
#include <flake/volume/arrows/vf/format.hpp>
#include <flake/volume/arrows/vf/format_part.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vf/dynamic/make_part_index.hpp>
#include <fcppt/math/dim/object_impl.hpp>


flake::volume::arrows::object::object(
	sge::opencl::context::object &_context,
	arrows::manager &_arrows_manager,
	flakelib::volume::conversion::object &_conversion,
	flakelib::volume::conversion::arrow_scale const &_arrow_scale,
	flakelib::volume::conversion::grid_scale const &_grid_scale,
	flakelib::volume::conversion::origin const &_origin,
	flakelib::volume::grid_size const &_grid_size)
:
	renderer_(
		_arrows_manager.renderer_),
	conversion_(
		_conversion),
	arrow_scale_(
		_arrow_scale),
	grid_scale_(
		_grid_scale),
	origin_(
		_origin),
	vb_(
		renderer_.create_vertex_buffer(
			*_arrows_manager.vertex_declaration_,
			sge::renderer::vf::dynamic::make_part_index
			<
				vf::format,
				vf::format_part
			>(),
			sge::renderer::vertex_count(
				_grid_size.get().content() * 2u),
			sge::renderer::resource_flags::none)),
	gl_buffer_(
		_context,
		*vb_,
		sge::opencl::memory_object::renderer_buffer_lock_mode::write_only)
{
	_arrows_manager.add_child(
		*this);
}

void
flake::volume::arrows::object::update(
	flakelib::volume::velocity_buffer_view const &_velocity)
{
	conversion_.to_arrow_vb(
		_velocity.get(),
		gl_buffer_,
		arrow_scale_,
		origin_,
		grid_scale_);
}

void
flake::volume::arrows::object::render()
{
	sge::renderer::scoped_vertex_buffer scoped_vb(
		renderer_,
		*vb_);

	renderer_.render_nonindexed(
		sge::renderer::first_vertex(
			0u),
		sge::renderer::vertex_count(
			vb_->size()),
		sge::renderer::nonindexed_primitive_type::line);
}

flake::volume::arrows::object::~object()
{
}
