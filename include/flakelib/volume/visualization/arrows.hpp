#ifndef FLAKELIB_VOLUME_VISUALIZATION_ARROWS_HPP_INCLUDED
#define FLAKELIB_VOLUME_VISUALIZATION_ARROWS_HPP_INCLUDED

#include <flakelib/volume/conversion/object_fwd.hpp>
#include <flakelib/volume/conversion/arrow_scale.hpp>
#include <flakelib/volume/visualization/arrows_manager_fwd.hpp>
#include <flakelib/buffer/volume_view_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <flakelib/volume/visualization/grid_size.hpp>
#include <flakelib/volume/conversion/grid_scale.hpp>
#include <sge/renderer/vertex_declaration_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace volume
{
namespace visualization
{
class arrows
{
FCPPT_NONCOPYABLE(
	arrows);
public:
	explicit
	arrows(
		sge::opencl::context::object &,
		visualization::arrows_manager &,
		volume::conversion::object &,
		visualization::grid_size const &,
		volume::conversion::arrow_scale const &,
		volume::conversion::grid_scale const &);

	void
	convert(
		buffer::volume_view<cl_float4> const &);

	void
	render();

	~arrows();
private:
	sge::renderer::device &renderer_;
	volume::conversion::object &conversion_;
	volume::conversion::arrow_scale const arrow_scale_;
	volume::conversion::grid_scale const grid_scale_;
	sge::renderer::vertex_buffer_ptr const vb_;
	sge::opencl::memory_object::buffer gl_buffer_;
};
}
}
}

#endif
