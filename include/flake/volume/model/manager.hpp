#ifndef FLAKE_VOLUME_MODEL_MANAGER_HPP_INCLUDED
#define FLAKE_VOLUME_MODEL_MANAGER_HPP_INCLUDED

#include <flake/volume/model/identifier.hpp>
#include <flake/volume/model/object.hpp>
#include <flake/volume/model/sun_direction.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/model/obj/loader_scoped_ptr.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/shader/object.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace volume
{
namespace model
{
class manager
{
FCPPT_NONCOPYABLE(
	manager);
public:
	manager(
		sge::renderer::device &,
		sge::image2d::system &,
		sge::camera::base &,
		model::sun_direction const &);

	void
	render();

	~manager();
private:
	friend class model::object;

	typedef
	boost::ptr_map
	<
		model::identifier,
		sge::renderer::vertex_buffer
	>
	vertex_buffer_map;

	typedef
	boost::ptr_map
	<
		model::identifier,
		sge::renderer::texture::planar
	>
	texture_map;

	typedef
	boost::intrusive::list
	<
		model::object,
		boost::intrusive::constant_time_size<false>
	>
	child_sequence;

	sge::renderer::device &renderer_;
	sge::camera::base &camera_;
	sge::renderer::vertex_declaration_scoped_ptr vertex_declaration_;
	sge::shader::object shader_;
	vertex_buffer_map identifier_to_vertex_buffer_;
	texture_map identifier_to_texture_;
	sge::model::obj::loader_scoped_ptr model_loader_;
	child_sequence children_;

	void
	add_child(
		model::object &);
};
}
}
}

#endif

