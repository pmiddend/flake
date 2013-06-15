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
#include <sge/image/algorithm/may_overlap.hpp>
#include <sge/image2d/l8.hpp>
#include <sge/image2d/save_from_view.hpp>
#include <sge/image2d/algorithm/copy_and_convert.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image2d/view/object.hpp>
#include <sge/image2d/view/to_const.hpp>
#include <sge/renderer/lock_mode.hpp>
#include <sge/renderer/primitive_type.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/vector2.hpp>
#include <sge/renderer/context/core.hpp>
#include <sge/renderer/device/core.hpp>
#include <sge/renderer/state/core/blend/combined.hpp>
#include <sge/renderer/state/core/blend/object.hpp>
#include <sge/renderer/state/core/blend/parameters.hpp>
#include <sge/renderer/state/core/blend/scoped.hpp>
#include <sge/renderer/state/core/blend/write_mask_all.hpp>
#include <sge/renderer/state/core/rasterizer/object.hpp>
#include <sge/renderer/state/core/rasterizer/object_scoped_ptr.hpp>
#include <sge/renderer/state/core/rasterizer/parameters.hpp>
#include <sge/renderer/state/core/rasterizer/scoped.hpp>
#include <sge/renderer/state/core/sampler/object.hpp>
#include <sge/renderer/state/core/sampler/parameters.hpp>
#include <sge/renderer/state/core/sampler/scoped.hpp>
#include <sge/renderer/state/core/sampler/address/mode_all.hpp>
#include <sge/renderer/state/core/sampler/filter/linear.hpp>
#include <sge/renderer/texture/const_scoped_planar_lock.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/planar_parameters.hpp>
#include <sge/renderer/texture/scoped_planar_lock.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/vertex/buffer.hpp>
#include <sge/renderer/vertex/buffer_parameters.hpp>
#include <sge/renderer/vertex/declaration.hpp>
#include <sge/renderer/vertex/declaration_parameters.hpp>
#include <sge/renderer/vertex/scoped_buffer.hpp>
#include <sge/renderer/vertex/scoped_declaration.hpp>
#include <sge/renderer/vertex/scoped_lock.hpp>
#include <sge/renderer/vf/vertex.hpp>
#include <sge/renderer/vf/view.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/dynamic/make_part_index.hpp>
#include <sge/shader/scoped_pair.hpp>
#include <fcppt/make_cref.hpp>
#include <fcppt/assign/make_map.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/box/contains_point.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/config/external_begin.hpp>
#include <algorithm>
#include <fcppt/config/external_end.hpp>


flake::volume::density_visualization::raycaster::object::object(
	sge::renderer::device::core &_renderer,
	sge::shader::context &_shader_context,
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
	blend_state_(
		renderer_.create_blend_state(
			sge::renderer::state::core::blend::parameters(
				sge::renderer::state::core::blend::alpha_enabled(
					sge::renderer::state::core::blend::combined(
						sge::renderer::state::core::blend::source::src_alpha,
						sge::renderer::state::core::blend::dest::inv_src_alpha)),
				sge::renderer::state::core::blend::write_mask_all()))),
	sampler_state_(
		renderer_.create_sampler_state(
			sge::renderer::state::core::sampler::parameters(
				sge::renderer::state::core::sampler::address::mode_all(
					sge::renderer::state::core::sampler::address::mode::clamp),
				sge::renderer::state::core::sampler::filter::linear()))),
	grid_size_(
		_grid_size),
	debug_output_(
		_debug_output),
	vertex_declaration_(
		_renderer.create_vertex_declaration(
			sge::renderer::vertex::declaration_parameters(
				sge::renderer::vf::dynamic::make_format<vf::format>()))),
	vertex_buffer_(
		renderer_.create_vertex_buffer(
			sge::renderer::vertex::buffer_parameters(
				*vertex_declaration_,
				sge::renderer::vf::dynamic::make_part_index
				<
					vf::format,
					vf::format_part
				>(),
				sge::renderer::vertex::count(
					36u),
				sge::renderer::resource_flags_field::null()))),
	texture_(
		renderer_.create_planar_texture(
			sge::renderer::texture::planar_parameters(
				fcppt::math::dim::structure_cast<sge::renderer::dim2>(
					flakelib::volume::conversion::planar_size_from_volume_size(
						_grid_size.get())),
				sge::renderer::texture::color_format(
					sge::image::color::format::r32f,
					sge::renderer::texture::emulate_srgb::no),
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field(
			//		sge::renderer::resource_flags_field::null()),
					sge::renderer::resource_flags::readable),
				sge::renderer::texture::capabilities_field::null()))),
	cl_texture_(
		_context,
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) |
			sge::opencl::memory_object::flags::write,
		*texture_),
	shader_(
		_shader_context,
		*vertex_declaration_,
		sge::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/raycaster.cg"))),
		sge::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/raycaster.cg"))),
		sge::shader::optional_cflags()),
	mvp_parameter_(
		shader_.vertex_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"mvp")),
		renderer_,
		sge::shader::parameter::is_projection_matrix(
			true),
		sge::renderer::matrix4::identity()),
	step_size_parameter_(
		shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"step_size")),
		_step_size.get()),
	elements_per_row_parameter_(
		shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"elements_per_row")),
		static_cast<int>(
			texture_->size().w() / _grid_size.get().w())),
	camera_is_inside_cube_parameter_(
		shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"camera_is_inside_cube")),
		sge::renderer::scalar()),
	slice_width_vertex_parameter_(
		shader_.vertex_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"slice_width")),
		static_cast<sge::renderer::scalar>(
			_grid_size.get().w())),
	slice_width_pixel_parameter_(
		shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"slice_width")),
		static_cast<int>(
			_grid_size.get().w())),
	texture_size_parameter_(
		shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"texture_size")),
		sge::renderer::vector2(
			static_cast<sge::renderer::scalar>(
				texture_->size().w()),
			static_cast<sge::renderer::scalar>(
				texture_->size().h()))),
	camera_position_parameter_(
		shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"camera_position")),
		sge::renderer::vector3::null()),
	loaded_texture_(
		shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"density_texture")),
		shader_,
		renderer_,
		sge::shader::parameter::planar_texture::optional_value(
			*texture_))
{
	sge::renderer::vertex::scoped_lock vblock(
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
flake::volume::density_visualization::raycaster::object::render(
	sge::renderer::context::core &_context)
{
	sge::renderer::vertex::scoped_declaration scoped_vd(
		_context,
		*vertex_declaration_);

	sge::shader::scoped_pair scoped_shader(
		_context,
		shader_);

	mvp_parameter_.set(
		sge::camera::matrix_conversion::world_projection(
			camera_.coordinate_system(),
			camera_.projection_matrix()));

	camera_position_parameter_.set(
		-camera_.coordinate_system().position().get());

	camera_is_inside_cube_parameter_.set(
		this->camera_is_inside_cube()
		?
			static_cast<sge::renderer::scalar>(
				1.0f)
		:
			static_cast<sge::renderer::scalar>(
				0.0f));

	sge::renderer::state::core::blend::scoped scoped_blend_state(
		_context,
		*blend_state_);

	sge::renderer::state::core::rasterizer::object_scoped_ptr const rasterizer_state(
		renderer_.create_rasterizer_state(
			sge::renderer::state::core::rasterizer::parameters(
				this->camera_is_inside_cube()
				?
					sge::renderer::state::core::rasterizer::cull_mode::counter_clockwise
				:
					sge::renderer::state::core::rasterizer::cull_mode::clockwise,
				sge::renderer::state::core::rasterizer::fill_mode::solid,
				sge::renderer::state::core::rasterizer::enable_scissor_test(
					false))));

	sge::renderer::state::core::rasterizer::scoped scoped_rasterizer_state(
		_context,
		*rasterizer_state);

	sge::renderer::state::core::sampler::scoped scoped_sampler_state(
		_context,
		fcppt::assign::make_map<sge::renderer::state::core::sampler::const_object_ref_map>
			(
				sge::renderer::texture::stage(
					0u),
				fcppt::make_cref(
					*sampler_state_)));

	sge::renderer::vertex::scoped_buffer scoped_vb(
		_context,
		*vertex_buffer_);

	_context.render_nonindexed(
		sge::renderer::vertex::first(
			0u),
		sge::renderer::vertex::count(
			36u),
		sge::renderer::primitive_type::triangle_list);
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
					fcppt::math::dim::static_<sge::renderer::scalar,3>
				>(
					grid_size_.get())),
			-camera_.coordinate_system().position().get());
}
