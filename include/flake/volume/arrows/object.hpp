#ifndef FLAKE_VOLUME_ARROWS_OBJECT_HPP_INCLUDED
#define FLAKE_VOLUME_ARROWS_OBJECT_HPP_INCLUDED

#include <flake/volume/arrows/manager_fwd.hpp>
#include <flakelib/volume/grid_size.hpp>
#include <flakelib/volume/velocity_buffer_view.hpp>
#include <flakelib/volume/conversion/arrow_scale.hpp>
#include <flakelib/volume/conversion/grid_scale.hpp>
#include <flakelib/volume/conversion/object_fwd.hpp>
#include <flakelib/volume/conversion/origin.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_buffer_scoped_ptr.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace volume
{
namespace arrows
{
class object
:
	public boost::intrusive::list_base_hook
	<
		boost::intrusive::link_mode<boost::intrusive::auto_unlink>
	>
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::opencl::context::object &,
		arrows::manager &,
		flakelib::volume::conversion::object &,
		flakelib::volume::conversion::arrow_scale const &,
		flakelib::volume::conversion::grid_scale const &,
		flakelib::volume::conversion::origin const &,
		flakelib::volume::grid_size const &);

	void
	update(
		flakelib::volume::velocity_buffer_view const &);

	void
	render();

	~object();
private:
	sge::renderer::device &renderer_;
	flakelib::volume::conversion::object &conversion_;
	flakelib::volume::conversion::arrow_scale const arrow_scale_;
	flakelib::volume::conversion::grid_scale const grid_scale_;
	flakelib::volume::conversion::origin const origin_;
	sge::renderer::vertex_buffer_scoped_ptr const vb_;
	sge::opencl::memory_object::buffer gl_buffer_;
};
}
}
}

#endif
