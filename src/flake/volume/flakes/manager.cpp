#include <sge/cg/parameter/scalar/set.hpp>
#include <sge/renderer/cg/loaded_texture.hpp>
#include <sge/renderer/cg/loaded_program.hpp>
#include <sge/renderer/vector2.hpp>
#include <sge/renderer/context/object.hpp>
#include <sge/renderer/cg/scoped_texture.hpp>
#include <sge/renderer/cg/scoped_program.hpp>
#include <sge/cg/program/from_file_parameters.hpp>
#include <flake/shader/load_edited_string.hpp>
#include <flake/media_path_from_string.hpp>
#include <flake/volume/flakes/manager.hpp>
#include <flake/volume/flakes/vf/format.hpp>
#include <flake/volume/flakes/vf/position_part.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/cg/parameter/vector/set.hpp>
#include <sge/cg/parameter/matrix/set.hpp>
#include <sge/cg/program/from_string_parameters.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration_and_buffers.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/dest_blend_func.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/state/source_blend_func.hpp>
#include <sge/renderer/texture/create_planar_from_path.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/vf/iterator.hpp>
#include <sge/renderer/vf/vertex.hpp>
#include <sge/renderer/vf/view.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/dynamic/make_part_index.hpp>
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
	sge::renderer::device &_renderer,
	sge::cg::context::object &_cg_context,
	flake::shader::vertex_profile const &_cg_vertex_profile,
	flake::shader::pixel_profile const &_cg_pixel_profile,
	sge::camera::base &_camera,
	sge::opencl::context::object &_context,
	sge::image2d::system &_image_system,
	flakes::count const &_flake_count,
	flakes::minimum_size const &_minimum_size,
	flakes::maximum_size const &_maximum_size,
	flakes::texture const &_texture,
	flakes::texture_tile_size const &_tile_size,
	flakes::texture_tile_count const &_tile_count,
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
			sge::renderer::resource_flags::none)),
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
			sge::renderer::resource_flags::none)),
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
			sge::renderer::resource_flags::none)),
	tile_size_(
		_tile_size),
	vertex_program_(
		sge::cg::program::from_string_parameters(
			_cg_context,
			sge::cg::program::source_type::text,
			_cg_vertex_profile.get(),
			flake::shader::load_edited_string(
				renderer_,
				*vertex_declaration_,
				flake::media_path_from_string(
					FCPPT_TEXT("shaders/flakes.cg"))),
			sge::cg::program::main_function(
				"vertex_main"),
			_renderer.cg_compile_options(
				_cg_context,
				_cg_vertex_profile.get()))),
	pixel_program_(
		sge::cg::program::from_string_parameters(
			_cg_context,
			sge::cg::program::source_type::text,
			_cg_pixel_profile.get(),
			flake::shader::load_edited_string(
				renderer_,
				*vertex_declaration_,
				flake::media_path_from_string(
					FCPPT_TEXT("shaders/flakes.cg"))),
			sge::cg::program::main_function(
				"pixel_main"),
			_renderer.cg_compile_options(
				_cg_context,
				_cg_pixel_profile.get()))),
	loaded_vertex_program_(
		renderer_.load_cg_program(
			vertex_program_)),
	loaded_pixel_program_(
		renderer_.load_cg_program(
			pixel_program_)),
	camera_position_parameter_(
		vertex_program_.parameter(
			"camera_position")),
	tile_size_parameter_(
		pixel_program_.parameter(
			"tile_size")),
	maximum_distance_parameter_(
		vertex_program_.parameter(
			"maximum_distance")),
	mvp_parameter_(
		vertex_program_.parameter(
			"mvp")),
	texture_(
		sge::renderer::texture::create_planar_from_path(
			flake::media_path_from_string(
				FCPPT_TEXT("textures/")+
				_texture.get()),
			renderer_,
			_image_system,
			sge::renderer::texture::mipmap::off(),
			sge::renderer::resource_flags::none)),
	loaded_texture_(
		renderer_.load_cg_texture(
			pixel_program_.parameter(
				"main_texture").object(),
			*texture_)),
	cl_positions_buffer_(),
	cl_point_sizes_buffer_()
{
	this->generate_particles(
		_grid_size,
		_tile_count);

	sge::cg::parameter::scalar::set(
		maximum_distance_parameter_.object(),
		static_cast<sge::renderer::scalar>(
			_grid_size.get().w()));

	sge::cg::parameter::scalar::set(
		tile_size_parameter_.object(),
		static_cast<sge::renderer::scalar>(
			_tile_size.get()));

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
	sge::renderer::context::object &_context)
{
	sge::renderer::cg::scoped_program
		scoped_vertex_program(
			_context,
			*loaded_vertex_program_),
		scoped_pixel_program(
			_context,
			*loaded_pixel_program_);

	sge::cg::parameter::matrix::set(
		mvp_parameter_.object(),
		sge::camera::matrix_conversion::world_projection(
			camera_.coordinate_system(),
			camera_.projection_matrix()));

	sge::cg::parameter::vector::set(
		camera_position_parameter_.object(),
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

	sge::renderer::state::scoped scoped_state(
		_context,
		sge::renderer::state::list
			(sge::renderer::state::bool_::enable_point_sprites = true)
			(sge::renderer::state::source_blend_func::src_alpha)
			(sge::renderer::state::dest_blend_func::inv_src_alpha)
			(sge::renderer::state::depth_func::less)
			(sge::renderer::state::bool_::write_to_depth_buffer = false)
			(sge::renderer::state::bool_::enable_alpha_blending = true));

	sge::renderer::cg::scoped_texture scoped_texture(
		_context,
		*loaded_texture_);

	_context.render_nonindexed(
		sge::renderer::first_vertex(
			0u),
		sge::renderer::vertex_count(
			positions_buffer_->size()),
		sge::renderer::nonindexed_primitive_type::point);
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

flake::volume::flakes::minimum_size const &
flake::volume::flakes::manager::minimum_size() const
{
	return
		minimum_size_;
}

flake::volume::flakes::maximum_size const &
flake::volume::flakes::manager::maximum_size() const
{
	return
		maximum_size_;
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
						_grid_size.get().h()/5u)),
				renderer_scalar_distribution::sup(
					static_cast<sge::renderer::scalar>(
						_grid_size.get().h()/2u+10u)))),
		second_y_rng(
			number_generator,
			renderer_scalar_distribution(
				renderer_scalar_distribution::min(
					3.0f),
				renderer_scalar_distribution::sup(
					static_cast<sge::renderer::scalar>(
						_grid_size.get().h()/2u+10u)))),
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

		if(starting_position.x() < static_cast<sge::renderer::scalar>(_grid_size.get().w()))
			starting_position.y() =
				second_y_rng();

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
