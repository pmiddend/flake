#ifndef FLAKE_VOLUME_MODEL_MANAGER_HPP_INCLUDED
#define FLAKE_VOLUME_MODEL_MANAGER_HPP_INCLUDED

#include <flake/volume/model/fog_color.hpp>
#include <flake/volume/model/fog_density.hpp>
#include <flake/volume/model/identifier.hpp>
#include <flake/volume/model/object.hpp>
#include <flake/volume/model/sun_direction.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/image2d/system_fwd.hpp>
	//#include <sge/model/obj/loader_scoped_ptr.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/context/object_fwd.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/shader/context_fwd.hpp>
#include <sge/shader/pair.hpp>
#include <sge/shader/pixel_profile.hpp>
#include <sge/shader/vertex_profile.hpp>
#include <sge/shader/parameter/matrix.hpp>
#include <sge/shader/parameter/planar_texture.hpp>
#include <sge/shader/parameter/scalar.hpp>
#include <sge/shader/parameter/vector.hpp>
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
		sge::shader::context &,
		sge::image2d::system &,
		sge::camera::base &,
		flake::volume::model::sun_direction const &,
		flake::volume::model::fog_color const &,
		flake::volume::model::fog_density const &);

	void
	render(
		sge::renderer::context::object &);

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

	sge::shader::pair shader_;
	sge::shader::parameter::matrix<sge::renderer::scalar,4,4> mvp_parameter_;
	sge::shader::parameter::matrix<sge::renderer::scalar,4,4> world_parameter_;
	sge::shader::parameter::vector<sge::renderer::scalar,3> sun_direction_parameter_;
	sge::shader::parameter::vector<sge::renderer::scalar,3> fog_color_parameter_;
	sge::shader::parameter::scalar<sge::renderer::scalar> fog_density_parameter_;
	sge::shader::parameter::planar_texture loaded_texture_;

	vertex_buffer_map identifier_to_vertex_buffer_;
	texture_map identifier_to_texture_;
		//sge::model::obj::loader_scoped_ptr model_loader_;
	child_sequence children_;

	void
	add_child(
		model::object &);
};
}
}
}

#endif

