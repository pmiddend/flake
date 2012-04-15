#include <flake/media_path_from_string.hpp>
#include <flake/volume/flakes/manager.hpp>
#include <flake/volume/flakes/vf/format.hpp>
#include <flake/volume/flakes/vf/position_part.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
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
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/vector/output.hpp>
#include <fcppt/random/variate.hpp>
#include <fcppt/random/distribution/uniform_real.hpp>
#include <fcppt/random/generator/minstd_rand.hpp>
#include <fcppt/random/generator/seed_from_chrono.hpp>


flake::volume::flakes::manager::manager(
	sge::renderer::device &_renderer,
	sge::camera::base &_camera,
	sge::opencl::context::object &_context,
	sge::image2d::system &_image_system,
	flakes::count const &_flake_count,
	flakes::minimum_size const &_minimum_size,
	flakes::maximum_size const &_maximum_size,
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
	shader_(
		sge::shader::object_parameters(
			renderer_,
			*vertex_declaration_,
			sge::shader::vf_to_string<vf::format>(),
			fcppt::assign::make_container<sge::shader::variable_sequence>
				(sge::shader::variable(
					"camera_position",
					sge::shader::variable_type::uniform,
					sge::renderer::vector3()))
				(sge::shader::variable(
					"maximum_distance",
					sge::shader::variable_type::uniform,
					static_cast<sge::renderer::scalar>(
						_grid_size.get().w())))
				(sge::shader::variable(
					"mvp",
					sge::shader::variable_type::uniform,
					sge::shader::matrix(
						sge::renderer::matrix4(),
						sge::shader::matrix_flags::projection))),
			fcppt::assign::make_container<sge::shader::sampler_sequence>
				(sge::shader::sampler(
					"tex",
					sge::renderer::texture::planar_shared_ptr(
						sge::renderer::texture::create_planar_from_path(
							flake::media_path_from_string(FCPPT_TEXT("textures/flake.png")),
							renderer_,
							_image_system,
							sge::renderer::texture::mipmap::off(),
							sge::renderer::resource_flags::none)))))
			.vertex_shader(
				flake::media_path_from_string(
					FCPPT_TEXT("shaders/flakes/vertex.glsl")))
			.fragment_shader(
				flake::media_path_from_string(
					FCPPT_TEXT("shaders/flakes/fragment.glsl")))
			.name(
				FCPPT_TEXT("Flakes"))),
	cl_positions_buffer_(),
	cl_point_sizes_buffer_()
{
	this->generate_particles(
		_grid_size);

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
flake::volume::flakes::manager::render()
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

	sge::renderer::scoped_vertex_buffer scoped_positions_vb(
		renderer_,
		*positions_buffer_);

	sge::renderer::scoped_vertex_buffer scoped_point_sizes_vb(
		renderer_,
		*point_sizes_buffer_);

	sge::renderer::state::scoped scoped_state(
		renderer_,
		sge::renderer::state::list
			(sge::renderer::state::bool_::enable_point_sprites = true)
			(sge::renderer::state::source_blend_func::src_alpha)
			(sge::renderer::state::dest_blend_func::inv_src_alpha)
			(sge::renderer::state::depth_func::less)
			(sge::renderer::state::bool_::write_to_depth_buffer = false)
			(sge::renderer::state::bool_::enable_alpha_blending = true));

	renderer_.render_nonindexed(
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
	flakelib::volume::grid_size const &_grid_size)
{
	sge::renderer::scoped_vertex_lock const positions_vblock(
		*positions_buffer_,
		sge::renderer::lock_mode::writeonly);

	sge::renderer::scoped_vertex_lock const point_sizes_vblock(
		*point_sizes_buffer_,
		sge::renderer::lock_mode::writeonly);

	sge::renderer::vf::view<vf::position_part> const positions(
		positions_vblock.value());

	sge::renderer::vf::view<vf::point_size_part> const point_sizes(
		point_sizes_vblock.value());

	sge::renderer::vf::view<vf::position_part>::iterator positions_it(
		positions.begin());

	sge::renderer::vf::view<vf::point_size_part>::iterator point_sizes_it(
		point_sizes.begin());

	typedef
	fcppt::random::generator::minstd_rand
	generator_type;

	generator_type number_generator(
		fcppt::random::generator::seed_from_chrono<generator_type::seed>());

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
	}
}
