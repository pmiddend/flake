#ifndef FLAKELIB_VOLUME_VISUALIZATION_SHAPE_MANAGER_HPP_INCLUDED
#define FLAKELIB_VOLUME_VISUALIZATION_SHAPE_MANAGER_HPP_INCLUDED

#include <flakelib/volume/boundary/cube/object.hpp>
#include <flakelib/volume/visualization/compiled_model.hpp>
#include <flakelib/volume/boundary/sphere/object.hpp>
#include <sge/model/obj/loader_ptr.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/shader/object.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <vector>

namespace flakelib
{
namespace volume
{
namespace visualization
{
class shape_manager
{
public:
	explicit
	shape_manager(
		sge::renderer::device &,
		sge::image2d::system &);

	void
	render(
		sge::renderer::matrix4 const &);

	void
	add(
		boundary::sphere::object const &);

	void
	add(
		boundary::cube::object const &);

	~shape_manager();
private:
	typedef
	std::vector<boundary::sphere::object>
	sphere_sequence;

	typedef
	std::vector<boundary::cube::object>
	cube_sequence;

	sge::renderer::device &renderer_;
	sge::model::obj::loader_ptr obj_loader_;
	sge::renderer::vertex_declaration_ptr vd_;
	sge::shader::object shader_;
	visualization::compiled_model sphere_model_;
	visualization::compiled_model cube_model_;
	sphere_sequence spheres_;
	cube_sequence cubes_;
};
}
}
}

#endif
