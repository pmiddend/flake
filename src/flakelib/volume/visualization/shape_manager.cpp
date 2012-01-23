#include <flakelib/media_path_from_string.hpp>
#include <flakelib/volume/visualization/shape_manager.hpp>
#include <flakelib/volume/visualization/model_vf/format.hpp>
#include <sge/model/obj/create.hpp>
#include <sge/model/obj/loader.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/texture/create_planar_from_path.hpp>
#include <sge/renderer/texture/mipmap/all_levels.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/shader/activate_bare.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/variable.hpp>
#include <sge/shader/variable_sequence.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <fcppt/assert/unreachable.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/matrix/arithmetic.hpp>
#include <fcppt/math/matrix/scaling.hpp>
#include <fcppt/math/matrix/translation.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/variant/get.hpp>
#include <fcppt/variant/holds_type.hpp>


flakelib::volume::visualization::shape_manager::shape_manager(
	sge::renderer::device &_renderer,
	sge::image2d::system &_image_loader,
	boundary::obstacle_sequence const &_obstacles,
	visualization::movement_hack const &_movement_hack,
	visualization::scaling_hack const &_scaling_hack)
:
	renderer_(
		_renderer),
	movement_hack_(
		_movement_hack.get()),
	scaling_hack_(
		_scaling_hack.get()),
	obj_loader_(
		sge::model::obj::create()),
	vd_(
		renderer_.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<model_vf::format>())),
	shader_(
		sge::shader::object_parameters(
			renderer_,
			*vd_,
			sge::shader::vf_to_string<model_vf::format>(),
			fcppt::assign::make_container<sge::shader::variable_sequence>
				(sge::shader::variable(
					"mvp",
					sge::shader::variable_type::uniform,
					sge::shader::matrix(
						sge::renderer::matrix4::identity(),
						sge::shader::matrix_flags::projection))),
			fcppt::assign::make_container<sge::shader::sampler_sequence>
				(sge::shader::sampler(
					"primary_texture",
					sge::renderer::texture::planar_ptr())))
				.vertex_shader(
					flakelib::media_path_from_string(
						FCPPT_TEXT("shaders/model/vertex.glsl")))
				.fragment_shader(
					flakelib::media_path_from_string(
						FCPPT_TEXT("shaders/model/fragment.glsl")))),
	sphere_model_(
		obj_loader_->load(
			flakelib::media_path_from_string(
				FCPPT_TEXT("models/sphere.obj"))),
		renderer_,
		*vd_,
		sge::renderer::texture::create_planar_from_path(
			flakelib::media_path_from_string(
				FCPPT_TEXT("models/sphere.png")),
			renderer_,
			_image_loader,
			sge::renderer::texture::mipmap::all_levels(
				sge::renderer::texture::mipmap::auto_generate::yes),
			sge::renderer::resource_flags::none)),
	cube_model_(
		obj_loader_->load(
			flakelib::media_path_from_string(
				FCPPT_TEXT("models/cube.obj"))),
		renderer_,
		*vd_,
		sge::renderer::texture::create_planar_from_path(
			flakelib::media_path_from_string(
				FCPPT_TEXT("models/cube.png")),
			renderer_,
			_image_loader,
			sge::renderer::texture::mipmap::all_levels(
				sge::renderer::texture::mipmap::auto_generate::yes),
			sge::renderer::resource_flags::none)),
	spheres_(),
	cubes_()
{
	for(
		boundary::obstacle_sequence::const_iterator it =
			_obstacles.begin();
		it != _obstacles.end();
		++it)
	{
		// This could be a visitor, but I'm too lazy
		if(fcppt::variant::holds_type<boundary::sphere::object>(*it))
		{
			this->add(
				fcppt::variant::get<boundary::sphere::object>(
					*it));
			continue;
		}
		else if(fcppt::variant::holds_type<boundary::cube::object>(*it))
		{
			this->add(
				fcppt::variant::get<boundary::cube::object>(
					*it));
			continue;

		}

		FCPPT_ASSERT_UNREACHABLE;
	}
}

void
flakelib::volume::visualization::shape_manager::render(
	sge::renderer::matrix4 const &_mvp)
{
	sge::renderer::scoped_vertex_declaration scoped_vd(
		renderer_,
		*vd_);

	sge::shader::scoped scoped_shader(
		shader_,
		sge::shader::activate_bare());

	for(
		sphere_sequence::const_iterator it =
			spheres_.begin();
		it != spheres_.end();
		++it)
	{
		sge::renderer::vector3 scaling_vector(
			static_cast<sge::renderer::scalar>(
				it->radius().get()),
			static_cast<sge::renderer::scalar>(
				it->radius().get()),
			static_cast<sge::renderer::scalar>(
				it->radius().get()));

		if(scaling_hack_)
			scaling_vector *=
				static_cast<sge::renderer::scalar>(
					 1.0/64.0);

		shader_.update_uniform(
			"mvp",
			sge::shader::matrix(
				_mvp *
				fcppt::math::matrix::translation(
					fcppt::math::dim::structure_cast<sge::renderer::vector3>(
						it->position().get())) *
				fcppt::math::matrix::scaling(
					scaling_vector),
				sge::shader::matrix_flags::projection));

		sphere_model_.render();
	}

	for(
		cube_sequence::const_iterator it =
			cubes_.begin();
		it != cubes_.end();
		++it)
	{
		sge::renderer::vector3 const scaling_vector(
			static_cast<sge::renderer::scalar>(
				it->size().get()[0]),
			static_cast<sge::renderer::scalar>(
				it->size().get()[1]),
			static_cast<sge::renderer::scalar>(
				it->size().get()[2]));

		sge::renderer::vector3 const translation =
			movement_hack_
			?
				sge::renderer::vector3(
						static_cast<sge::renderer::scalar>(
							it->position().get()[0]-1),
						static_cast<sge::renderer::scalar>(
							it->position().get()[1]),
						static_cast<sge::renderer::scalar>(
							it->position().get()[2]))
			:
				sge::renderer::vector3(
						static_cast<sge::renderer::scalar>(
							it->position().get()[0]),
						static_cast<sge::renderer::scalar>(
							it->position().get()[1]),
						static_cast<sge::renderer::scalar>(
							it->position().get()[2]));

		shader_.update_uniform(
			"mvp",
			sge::shader::matrix(
				_mvp *
				fcppt::math::matrix::translation(
					translation +
					scaling_vector / static_cast<sge::renderer::scalar>(2)) *
				fcppt::math::matrix::scaling(
					scaling_vector),
				sge::shader::matrix_flags::projection));

		cube_model_.render();
	}
}

void
flakelib::volume::visualization::shape_manager::add(
	boundary::sphere::object const &_sphere)
{
	spheres_.push_back(
		_sphere);
}

void
flakelib::volume::visualization::shape_manager::add(
	boundary::cube::object const &_cube)
{
	cubes_.push_back(
		_cube);
}

sge::renderer::vertex_declaration const &
flakelib::volume::visualization::shape_manager::vertex_declaration() const
{
	return *vd_;
}

sge::shader::object &
flakelib::volume::visualization::shape_manager::shader()
{
	return shader_;
}

flakelib::volume::visualization::shape_manager::~shape_manager()
{
}
