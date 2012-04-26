#include <flake/media_path_from_string.hpp>
#include <flake/volume/density_visualization/raycaster/object.hpp>
#include <flake/volume/density_visualization/raycaster/vf/format.hpp>
#include <flake/volume/density_visualization/raycaster/vf/format_part.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <flakelib/volume/conversion/planar_size_from_volume_size.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/image/store.hpp>
#include <sge/image2d/l8.hpp>
#include <sge/image2d/save_from_view.hpp>
#include <sge/image2d/algorithm/copy_and_convert.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image2d/view/object.hpp>
#include <sge/image2d/view/to_const.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/state/dest_blend_func.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/state/source_blend_func.hpp>
#include <sge/renderer/texture/address_mode2.hpp>
#include <sge/renderer/texture/const_scoped_planar_lock.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/planar_parameters.hpp>
#include <sge/renderer/texture/scoped_planar_lock.hpp>
#include <sge/renderer/texture/set_address_mode2.hpp>
#include <sge/renderer/texture/filter/point.hpp>
#include <sge/renderer/texture/filter/linear.hpp>
#include <sge/renderer/texture/filter/scoped.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/vf/vertex.hpp>
#include <sge/renderer/vf/view.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/dynamic/make_part_index.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/box/contains_point.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/config/external_begin.hpp>
#include <algorithm>
#include <fcppt/config/external_end.hpp>


flake::volume::density_visualization::raycaster::object::object(
	sge::renderer::device &_renderer,
	sge::opencl::context::object &_context,
	sge::camera::base const &_camera,
	sge::image2d::system &_image_system,
	flakelib::volume::conversion::object &_conversion,
	flakelib::volume::grid_size const &_grid_size,
	flake::volume::density_visualization::raycaster::step_size const &_step_size,
	flake::volume::density_visualization::raycaster::debug_output const &_debug_output)
:
	renderer_(
		_renderer),
	camera_(
		_camera),
	image_system_(
		_image_system),
	conversion_(
		_conversion),
	grid_size_(
		_grid_size),
	debug_output_(
		_debug_output),
	vertex_declaration_(
		_renderer.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<vf::format>())),
	vertex_buffer_(
		renderer_.create_vertex_buffer(
			*vertex_declaration_,
			sge::renderer::vf::dynamic::make_part_index
			<
				vf::format,
				vf::format_part
			>(),
			sge::renderer::vertex_count(
				36u),
			sge::renderer::resource_flags::none)),
	texture_(
		renderer_.create_planar_texture(
			sge::renderer::texture::planar_parameters(
				fcppt::math::dim::structure_cast<sge::renderer::dim2>(
					flakelib::volume::conversion::planar_size_from_volume_size(
						_grid_size.get())),
				sge::image::color::format::r32f,
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field(
			//		sge::renderer::resource_flags::none),
					sge::renderer::resource_flags::readable),
				sge::renderer::texture::capabilities_field::null()))),
	cl_texture_(
		_context,
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) |
			sge::opencl::memory_object::flags::write,
		*texture_),
	shader_(
		sge::shader::object_parameters(
			renderer_,
			*vertex_declaration_,
			sge::shader::vf_to_string<vf::format>(),
			fcppt::assign::make_container<sge::shader::variable_sequence>
				(sge::shader::variable(
					"mvp",
					sge::shader::variable_type::uniform,
					sge::shader::matrix(
						sge::renderer::matrix4::identity(),
						sge::shader::matrix_flags::projection)))
				(sge::shader::variable(
					"step_size",
					sge::shader::variable_type::constant,
					_step_size.get()))
				(sge::shader::variable(
					"elements_per_row",
					sge::shader::variable_type::constant,
					static_cast<sge::renderer::glsl::uniform::int_type>(
						texture_->size().w() / _grid_size.get().w())))
				(sge::shader::variable(
					"camera_is_inside_cube",
					sge::shader::variable_type::uniform,
					sge::renderer::scalar()))
				(sge::shader::variable(
					"slice_width",
					sge::shader::variable_type::constant,
					static_cast<sge::renderer::glsl::uniform::int_type>(
						_grid_size.get().w())))
				(sge::shader::variable(
					"texture_size",
					sge::shader::variable_type::constant,
					sge::renderer::vector2(
						static_cast<sge::renderer::scalar>(
							texture_->size().w()),
						static_cast<sge::renderer::scalar>(
							texture_->size().h()))))
				(sge::shader::variable(
					"camera_position",
					sge::shader::variable_type::uniform,
					sge::renderer::vector3())),
			fcppt::assign::make_container<sge::shader::sampler_sequence>
				(sge::shader::sampler(
					"density_texture",
					texture_)))
			.vertex_shader(
				flake::media_path_from_string(
					FCPPT_TEXT("shaders/raycaster/vertex.glsl")))
			.fragment_shader(
				flake::media_path_from_string(
					FCPPT_TEXT("shaders/raycaster/fragment.glsl")))
			.name(
				FCPPT_TEXT("Raycaster")))
{
	sge::renderer::scoped_vertex_lock vblock(
		*vertex_buffer_,
		sge::renderer::lock_mode::writeonly);

	sge::renderer::vf::view<raycaster::vf::format_part> vertices(
		vblock.value());

	sge::renderer::vf::view<raycaster::vf::format_part>::iterator vb_it(
		vertices.begin());

	// Here be dragons...
	int a = -1;
	int b = 1;

	for (unsigned side = 0u; side < 6u; ++side)
	{
		for (int tri = 0; tri < 2; ++tri)
		{
			for (int i = 0; i < 3; ++i)
			{
				int const vert =
					(tri == 0) ? i : 2 - i;

				unsigned const x =
					side % 3u;

				int
					y =
						(vert == 0) ? a : b,
					z =
						(vert == 2) ? b : a;

				if(
					((side % 2 == 0) && tri == 1) ||
					((side % 2 == 1) && tri == 0))
					std::swap(
						y,
						z);

				vf::position::packed_type res(
					0.0f,
					0.0f,
					0.0f);

				res[x] =
					static_cast<sge::renderer::scalar>(
						(side > x) ? b : a);

				res[x == 0u ? 1u : 0u] =
					static_cast<sge::renderer::scalar>(
						y);

				res[x == 2u ? 1u : 2u] =
					static_cast<sge::renderer::scalar>(
						z);

				(*vb_it++).set<vf::position>(
					res);
			}
		}
	}
}

void
flake::volume::density_visualization::raycaster::object::update(
	flakelib::volume::float_view const &_view,
	flakelib::volume::conversion::scaling_factor const &_scaling_factor,
	flakelib::volume::conversion::constant_addition const &_constant_addition)
{
	conversion_.float_view_to_flat_volume_texture(
		_view,
		cl_texture_,
		_scaling_factor,
		_constant_addition);

	if(!debug_output_.get())
		return;

	sge::renderer::texture::const_scoped_planar_lock const slock(
		*texture_);

	typedef
	sge::image2d::l8
	store_type;

	store_type temp_store(
		store_type::dim(
			texture_->size().w(),
			texture_->size().h()));

	sge::image2d::algorithm::copy_and_convert(
		slock.value(),
		sge::image2d::view::object(
			temp_store.wrapped_view()),
		sge::image::algorithm::may_overlap::no);

	sge::image2d::save_from_view(
		image_system_,
		sge::image2d::view::to_const(
			sge::image2d::view::object(
				temp_store.wrapped_view())),
		"/tmp/volume_texture.png");
}

void
flake::volume::density_visualization::raycaster::object::render()
{
	sge::shader::scoped scoped_shader(
		shader_,
		sge::shader::activate_everything());

	shader_.update_uniform(
		"mvp",
		sge::shader::matrix(
			sge::camera::matrix_conversion::world_projection(
				camera_.coordinate_system(),
				camera_.projection_matrix()),
			sge::shader::matrix_flags::projection));

	shader_.update_uniform(
		"camera_position",
		-camera_.coordinate_system().position().get());

	shader_.update_uniform(
		"camera_is_inside_cube",
		this->camera_is_inside_cube()
		?
			static_cast<sge::renderer::scalar>(1.0f)
		:
			static_cast<sge::renderer::scalar>(0.0f));

	sge::renderer::state::scoped scoped_state(
		renderer_,
		sge::renderer::state::list
			(sge::renderer::state::bool_::enable_alpha_blending = true)
			(sge::renderer::state::source_blend_func::src_alpha)
			(
			 this->camera_is_inside_cube()
			 ?
				 sge::renderer::state::cull_mode::counter_clockwise
			 : sge::renderer::state::cull_mode::clockwise)
			(sge::renderer::state::dest_blend_func::inv_src_alpha));

	sge::renderer::texture::filter::scoped scoped_texture_filter(
		renderer_,
		sge::renderer::texture::stage(
			0u),
//		sge::renderer::texture::filter::point());
		sge::renderer::texture::filter::linear());

	sge::renderer::texture::set_address_mode2(
		renderer_,
		sge::renderer::texture::stage(
			0u),
		sge::renderer::texture::address_mode2(
			sge::renderer::texture::address_mode::clamp));

	sge::renderer::scoped_vertex_buffer scoped_vb(
		renderer_,
		*vertex_buffer_);

	renderer_.render_nonindexed(
		sge::renderer::first_vertex(
			0u),
		sge::renderer::vertex_count(
			36u),
		sge::renderer::nonindexed_primitive_type::triangle);
}

flake::volume::density_visualization::raycaster::object::~object()
{
}

bool
flake::volume::density_visualization::raycaster::object::camera_is_inside_cube() const
{
	return
		fcppt::math::box::contains_point(
			fcppt::math::box::object<sge::renderer::scalar,3>(
				sge::renderer::vector3::null(),
				fcppt::math::dim::structure_cast
				<
					fcppt::math::dim::static_<sge::renderer::scalar,3>::type
				>(
					grid_size_.get())),
			-camera_.coordinate_system().position().get());
}
