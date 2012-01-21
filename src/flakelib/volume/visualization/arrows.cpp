#include <flakelib/buffer/volume_view.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <flakelib/volume/visualization/arrows.hpp>
#include <flakelib/volume/visualization/arrows_manager.hpp>
#include <flakelib/volume/visualization/arrow_vf/format.hpp>
#include <flakelib/volume/visualization/arrow_vf/format_part.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vf/dynamic/make_part_index.hpp>
#include <fcppt/math/dim/basic_impl.hpp>


flakelib::volume::visualization::arrows::arrows(
	sge::opencl::context::object &_context,
	visualization::arrows_manager &_arrows_manager,
	volume::conversion::object &_conversion,
	visualization::grid_size const &_grid_size,
	volume::conversion::arrow_scale const &_arrow_scale)
:
	renderer_(
		_arrows_manager.renderer()),
	conversion_(
		_conversion),
	arrow_scale_(
		_arrow_scale),
	vb_(
		renderer_.create_vertex_buffer(
			_arrows_manager.vertex_declaration(),
			sge::renderer::vf::dynamic::make_part_index
			<
				arrow_vf::format,
				arrow_vf::format_part
			>(),
			sge::renderer::vertex_count(
				_grid_size.get().content() * 2u),
			sge::renderer::resource_flags::none)),
	gl_buffer_(
		_context,
		*vb_,
		sge::opencl::memory_object::renderer_buffer_lock_mode::read_write)
{
}

void
flakelib::volume::visualization::arrows::convert(
	buffer::volume_view<cl_float4> const &_b)
{
	conversion_.to_arrow_vb(
		flakelib::volume::conversion::cl_buffer(
			_b),
		flakelib::volume::conversion::gl_buffer(
			gl_buffer_),
		arrow_scale_);
}

void
flakelib::volume::visualization::arrows::render()
{
	sge::renderer::scoped_vertex_buffer scoped_vb(
		renderer_,
		*vb_);

	renderer_.render_nonindexed(
		sge::renderer::first_vertex(
			0),
		sge::renderer::vertex_count(
			vb_->size()),
		sge::renderer::nonindexed_primitive_type::line);
}

flakelib::volume::visualization::arrows::~arrows()
{
}
