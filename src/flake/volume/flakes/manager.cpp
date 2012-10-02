#include <flake/media_path_from_string.hpp>
#include <flake/volume/flakes/manager.hpp>
#include <flake/volume/flakes/vf/format.hpp>
#include <flake/volume/flakes/vf/position_part.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration_and_buffers.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/vector2.hpp>
#include <sge/renderer/vector4.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/context/ffp.hpp>
#include <sge/renderer/device/ffp.hpp>
#include <sge/renderer/state/core/blend/alpha_enabled.hpp>
#include <sge/renderer/state/core/blend/combined.hpp>
#include <sge/renderer/state/core/blend/object.hpp>
#include <sge/renderer/state/core/blend/parameters.hpp>
#include <sge/renderer/state/core/blend/scoped.hpp>
#include <sge/renderer/state/core/blend/write_mask_all.hpp>
#include <sge/renderer/state/core/depth_stencil/object.hpp>
#include <sge/renderer/state/core/depth_stencil/object_scoped_ptr.hpp>
#include <sge/renderer/state/core/depth_stencil/parameters.hpp>
#include <sge/renderer/state/core/depth_stencil/scoped.hpp>
#include <sge/renderer/state/ffp/misc/object.hpp>
#include <sge/renderer/state/ffp/misc/parameters.hpp>
#include <sge/renderer/state/ffp/misc/scoped.hpp>
#include <sge/renderer/texture/create_planar_from_path.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/vf/iterator.hpp>
#include <sge/renderer/vf/vertex.hpp>
#include <sge/renderer/vf/view.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/dynamic/make_part_index.hpp>
#include <sge/shader/load_edited_string.hpp>
#include <sge/shader/scoped_pair.hpp>
#include <fcppt/cref.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/vector/output.hpp>
#include <fcppt/random/variate.hpp>
#include <fcppt/random/distribution/uniform_int.hpp>
#include <fcppt/random/distribution/uniform_real.hpp>
#include <fcppt/random/generator/minstd_rand.hpp>
#include <fcppt/random/generator/seed_from_chrono.hpp>


flake::volume::flakes::manager::manager(
	sge::renderer::device::ffp &_renderer,
	sge::shader::context &_shader_context,
	sge::camera::base &_camera,
	sge::opencl::context::object &_context,
	sge::image2d::system &_image_system,
	flake::volume::flakes::count const &_flake_count,
	flake::volume::flakes::minimum_size const &_minimum_size,
	flake::volume::flakes::maximum_size const &_maximum_size,
	flake::volume::flakes::texture const &_texture,
	flake::volume::flakes::texture_tile_size const &_tile_size,
	flake::volume::flakes::texture_tile_count const &_tile_count,
	flakelib::volume::grid_size const &_grid_size)
:
	renderer_(
		_renderer),
	camera_(
		_camera),
	minimum_size_(
		_minimum_size),
	maximum_size_(
		_maximum_size),
	blend_state_(
		renderer_.create_blend_state(
			sge::renderer::state::core::blend::parameters(
				sge::renderer::state::core::blend::alpha_enabled(
					sge::renderer::state::core::blend::combined(
						sge::renderer::state::core::blend::source::src_alpha,
						sge::renderer::state::core::blend::dest::inv_src_alpha)),
				sge::renderer::state::core::blend::write_mask_all()))),
	depth_stencil_state_(
		renderer_.create_depth_stencil_state(
			sge::renderer::state::core::depth_stencil::parameters(
				sge::renderer::state::core::depth_stencil::depth::variant(
					sge::renderer::state::core::depth_stencil::depth::enabled(
						sge::renderer::state::core::depth_stencil::depth::func::less,
						sge::renderer::state::core::depth_stencil::depth::write_enable(
							false))),
				sge::renderer::state::core::depth_stencil::stencil::off()))),
	misc_state_(
		renderer_.create_misc_state(
			sge::renderer::state::ffp::misc::parameters(
				sge::renderer::state::ffp::misc::enable_point_sprites(
					true),
				sge::renderer::state::ffp::misc::local_viewer(
					false),
				sge::renderer::state::ffp::misc::normalize_normals(
					false)))),
	vertex_declaration_(
		renderer_.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<vf::format>())),
	positions_buffer_(
		renderer_.create_vertex_buffer(
			*vertex_declaration_,
			sge::renderer::vf::dynamic::make_part_index
			<
				vf::format,
				vf::position_part
			>(),
			sge::renderer::vertex_count(
				_flake_count.get()),
			sge::renderer::resource_flags_field::null())),
	texcoords_buffer_(
		renderer_.create_vertex_buffer(
			*vertex_declaration_,
			sge::renderer::vf::dynamic::make_part_index
			<
				vf::format,
				vf::texcoord_part
			>(),
			sge::renderer::vertex_count(
				_flake_count.get()),
			sge::renderer::resource_flags_field::null())),
	point_sizes_buffer_(
		renderer_.create_vertex_buffer(
			*vertex_declaration_,
			sge::renderer::vf::dynamic::make_part_index
			<
				vf::format,
				vf::point_size_part
			>(),
			sge::renderer::vertex_count(
				_flake_count.get()),
			sge::renderer::resource_flags_field::null())),
	tile_size_(
		_tile_size),
	shader_(
		_shader_context,
		*vertex_declaration_,
		sge::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/flakes.cg"))),
		sge::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/flakes.cg"))),
		sge::shader::optional_cflags()),
	camera_position_parameter_(
		shader_.vertex_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"camera_position")),
		sge::renderer::vector3()),
	tile_size_parameter_(
		shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"tile_size")),
		static_cast<sge::renderer::scalar>(
			_tile_size.get())),
	maximum_distance_parameter_(
		shader_.vertex_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"maximum_distance")),
		static_cast<sge::renderer::scalar>(
			_grid_size.get().w())),
	mvp_parameter_(
		shader_.vertex_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"mvp")),
		renderer_,
		sge::shader::parameter::is_projection_matrix(
			true),
		sge::renderer::matrix4()),
	texture_(
		sge::renderer::texture::create_planar_from_path(
			flake::media_path_from_string(
				FCPPT_TEXT("textures/")+
				_texture.get()),
			renderer_,
			_image_system,
			sge::renderer::texture::mipmap::off(),
			sge::renderer::resource_flags_field::null())),
	loaded_texture_(
		shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"main_texture")),
		shader_,
		renderer_,
		sge::shader::parameter::planar_texture::optional_value(
			*texture_)),
	cl_positions_buffer_(),
	cl_point_sizes_buffer_()
{
	this->generate_particles(
		_grid_size,
		_tile_count);

	cl_positions_buffer_.take(
		fcppt::make_unique_ptr<sge::opencl::memory_object::buffer>(
			fcppt::ref(
				_context),
			fcppt::ref(
				*positions_buffer_),
			sge::opencl::memory_object::renderer_buffer_lock_mode::read_write));

	cl_point_sizes_buffer_.take(
		fcppt::make_unique_ptr<sge::opencl::memory_object::buffer>(
			fcppt::ref(
				_context),
			fcppt::ref(
				*point_sizes_buffer_),
			sge::opencl::memory_object::renderer_buffer_lock_mode::read_write));
}

void
flake::volume::flakes::manager::render(
	sge::renderer::context::ffp &_context,
	flake::volume::flakes::count const &_count)
{
	sge::shader::scoped_pair scoped_shader(
		_context,
		shader_);

	mvp_parameter_.set(
		sge::camera::matrix_conversion::world_projection(
			camera_.coordinate_system(),
			camera_.projection_matrix()));

	camera_position_parameter_.set(
		-camera_.coordinate_system().position().get());

	sge::renderer::scoped_vertex_declaration_and_buffers scoped_vf(
		_context,
		*vertex_declaration_,
		fcppt::assign::make_container<sge::renderer::const_vertex_buffer_ref_container>
			(fcppt::cref(
				*positions_buffer_))
			(fcppt::cref(
				*point_sizes_buffer_))
			(fcppt::cref(
				*texcoords_buffer_)));

	sge::renderer::state::ffp::misc::scoped const scoped_misc(
		_context,
		*misc_state_);

	sge::renderer::state::core::blend::scoped scoped_blend_state(
		_context,
		*blend_state_);

	sge::renderer::state::core::depth_stencil::scoped scoped_depth_stencil_state(
		_context,
		*depth_stencil_state_);

	_context.render_nonindexed(
		sge::renderer::first_vertex(
			0u),
		sge::renderer::vertex_count(
			_count.get()),
		sge::renderer::primitive_type::point_list);
}

flake::volume::flakes::position_view const
flake::volume::flakes::manager::cl_positions()
{
	return
		flake::volume::flakes::position_view(
			flake::volume::flakes::position_view::value_type(
				*cl_positions_buffer_));
}

flake::volume::flakes::point_size_view const
flake::volume::flakes::manager::cl_point_sizes()
{
	return
		flake::volume::flakes::point_size_view(
			flake::volume::flakes::point_size_view::value_type(
				*cl_point_sizes_buffer_));
}

flake::volume::flakes::minimum_size
flake::volume::flakes::manager::minimum_size() const
{
	return
		minimum_size_;
}

flake::volume::flakes::maximum_size
flake::volume::flakes::manager::maximum_size() const
{
	return
		maximum_size_;
}

flake::volume::flakes::count
flake::volume::flakes::manager::maximum_count() const
{
	return
		flake::volume::flakes::count(
			positions_buffer_->size().get());
}

flake::volume::flakes::manager::~manager()
{
}

void
flake::volume::flakes::manager::generate_particles(
	flakelib::volume::grid_size const &_grid_size,
	flakes::texture_tile_count const &_number_of_textures)
{
	sge::renderer::scoped_vertex_lock const positions_vblock(
		*positions_buffer_,
		sge::renderer::lock_mode::writeonly);

	sge::renderer::scoped_vertex_lock const texcoords_vblock(
		*texcoords_buffer_,
		sge::renderer::lock_mode::writeonly);

	sge::renderer::scoped_vertex_lock const point_sizes_vblock(
		*point_sizes_buffer_,
		sge::renderer::lock_mode::writeonly);

	sge::renderer::vf::view<vf::position_part> const positions(
		positions_vblock.value());

	sge::renderer::vf::view<vf::texcoord_part> const texcoords(
		texcoords_vblock.value());

	sge::renderer::vf::view<vf::point_size_part> const point_sizes(
		point_sizes_vblock.value());

	sge::renderer::vf::view<vf::position_part>::iterator positions_it(
		positions.begin());

	sge::renderer::vf::view<vf::texcoord_part>::iterator texcoords_it(
		texcoords.begin());

	sge::renderer::vf::view<vf::point_size_part>::iterator point_sizes_it(
		point_sizes.begin());

	typedef
	fcppt::random::generator::minstd_rand
	generator_type;

	generator_type number_generator(
		fcppt::random::generator::seed_from_chrono<generator_type::seed>());

	typedef
	fcppt::random::distribution::uniform_int<unsigned>
	uniform_unsigned_distribution;

	typedef
	fcppt::random::distribution::uniform_real<sge::renderer::scalar>
	renderer_scalar_distribution;

	typedef
	fcppt::random::variate
	<
		generator_type,
		renderer_scalar_distribution
	>
	renderer_scalar_variate;

	typedef
	fcppt::random::variate
	<
		generator_type,
		uniform_unsigned_distribution
	>
	unsigned_variate;

	unsigned_variate part_rng(
		number_generator,
		uniform_unsigned_distribution(
			uniform_unsigned_distribution::min(
				0u),
			uniform_unsigned_distribution::max(
				_number_of_textures.get()-1u)));

	renderer_scalar_variate
		size_rng(
			number_generator,
			renderer_scalar_distribution(
				renderer_scalar_distribution::min(
					minimum_size_.get()),
				renderer_scalar_distribution::sup(
					maximum_size_.get()))),
		x_rng(
			number_generator,
			renderer_scalar_distribution(
				renderer_scalar_distribution::min(
					1.0f),
				renderer_scalar_distribution::sup(
					static_cast<sge::renderer::scalar>(
						_grid_size.get().w()/2u-1u)))),
		y_rng(
			number_generator,
			renderer_scalar_distribution(
				renderer_scalar_distribution::min(
					static_cast<sge::renderer::scalar>(
						1.0f)),
				renderer_scalar_distribution::sup(
					static_cast<sge::renderer::scalar>(
						_grid_size.get().h())-
					static_cast<sge::renderer::scalar>(
						1.0f)))),
		z_rng(
			number_generator,
			renderer_scalar_distribution(
				renderer_scalar_distribution::min(
					1.0f),
				renderer_scalar_distribution::sup(
					static_cast<sge::renderer::scalar>(
						_grid_size.get().d()-1u))));

	for(
		sge::renderer::vertex_count i(
			0u);
		i != positions_buffer_->size();
		++i)
	{
		sge::renderer::vector4 starting_position =
			sge::renderer::vector4(
				x_rng(),
				y_rng(),
				z_rng(),
				1.0f);

		sge::renderer::scalar const flake_size =
			size_rng();

		(*point_sizes_it++).set<vf::point_size>(
			vf::point_size::packed_type(
				flake_size));

		(*positions_it++).set<vf::position>(
			starting_position);

		sge::renderer::vector2 const new_texcoord(
			static_cast<sge::renderer::scalar>(
				part_rng()) *
			tile_size_.get(),
			0.0f);

		(*texcoords_it++).set<vf::texcoord>(
			new_texcoord);
	}
}
