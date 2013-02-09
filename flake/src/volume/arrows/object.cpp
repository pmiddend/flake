#include <flake/volume/arrows/manager.hpp>
#include <flake/volume/arrows/object.hpp>
#include <flake/volume/arrows/vf/format.hpp>
#include <flake/volume/arrows/vf/format_part.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <sge/opencl/memory_object/renderer_buffer_lock_mode.hpp>
#include <sge/renderer/primitive_type.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/context/core.hpp>
#include <sge/renderer/device/core.hpp>
#include <sge/renderer/vertex/buffer.hpp>
#include <sge/renderer/vertex/buffer_parameters.hpp>
#include <sge/renderer/vertex/scoped_buffer.hpp>
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
			sge::renderer::vertex::buffer_parameters(
				*_arrows_manager.vertex_declaration_,
				sge::renderer::vf::dynamic::make_part_index
				<
					vf::format,
					vf::format_part
				>(),
				sge::renderer::vertex::count(
					_grid_size.get().content() * 2u),
				sge::renderer::resource_flags_field::null()))),
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
	flakelib::volume::float4_view const &_arrows)
{
	conversion_.to_arrow_vb(
		_arrows,
		gl_buffer_,
		arrow_scale_,
		origin_,
		grid_scale_);
}

void
flake::volume::arrows::object::render(
	sge::renderer::context::core &_context)
{
	sge::renderer::vertex::scoped_buffer scoped_vb(
		_context,
		*vb_);

	_context.render_nonindexed(
		sge::renderer::vertex::first(
			0u),
		sge::renderer::vertex::count(
			vb_->size()),
		sge::renderer::primitive_type::line_list);
}

flake::volume::arrows::object::~object()
{
}
