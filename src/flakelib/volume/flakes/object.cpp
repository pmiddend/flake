#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/volume_view.hpp>
#include <flakelib/volume/flakes/object.hpp>
#include <flakelib/volume/flakes/vf/format.hpp>
#include <flakelib/volume/flakes/vf/point_size_part.hpp>
#include <flakelib/volume/flakes/vf/position_part.hpp>
#include <flakelib/volume/flakes/vf/starting_position_part.hpp>
#include <sge/camera/base.hpp>
#include <sge/image/colors.hpp>
#include <sge/image2d/file.hpp>
#include <sge/image2d/system.hpp>
#include <sge/image2d/algorithm/fill.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/texture/capabilities_field.hpp>
#include <sge/renderer/texture/create_planar_from_view.hpp>
#include <sge/renderer/texture/planar_parameters.hpp>
#include <sge/renderer/texture/scoped_planar_lock.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/vf/iterator.hpp>
#include <sge/renderer/vf/vertex.hpp>
#include <sge/renderer/vf/view.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/dynamic/part_index.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/update_single_uniform.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/unique_ptr.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/chrono/duration.hpp>
#include <fcppt/chrono/high_resolution_clock.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/container/ptr/replace_unique_ptr.hpp>
#include <fcppt/random/default_generator.hpp>
#include <fcppt/random/make_inclusive_range.hpp>
#include <fcppt/random/uniform.hpp>
#include <fcppt/config/external_begin.hpp>
#include <cstddef>
#include <fcppt/config/external_end.hpp>


flakelib::volume::flakes::object::object(
	sge::renderer::device &_renderer,
	sge::camera::base &_camera,
	sge::image2d::system &_image_system,
	sge::opencl::command_queue::object &_command_queue,
	boundary::view const &_boundary,
	flakelib::build_options const &_build_options,
	flakes::particle_count const &_particle_count,
	flakes::particle_minimum_size const &_particle_minimum_size,
	flakes::particle_maximum_size const &_particle_maximum_size,
	flakes::snow_texture_size const &_snow_texture_size)
:
	renderer_(
		_renderer),
	camera_(
		_camera),
	command_queue_(
		_command_queue),
	vertex_declaration_(
		renderer_.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<vf::format>())),
	positions_buffer_(
		renderer_.create_vertex_buffer(
			*vertex_declaration_,
			sge::renderer::vf::dynamic::part_index(
				0u),
			sge::renderer::vertex_count(
				_particle_count.get()),
			sge::renderer::resource_flags::none)),
	starting_positions_buffer_(
		renderer_.create_vertex_buffer(
			*vertex_declaration_,
			sge::renderer::vf::dynamic::part_index(
				1u),
			sge::renderer::vertex_count(
				_particle_count.get()),
			sge::renderer::resource_flags::none)),
	point_sizes_buffer_(
		renderer_.create_vertex_buffer(
			*vertex_declaration_,
			sge::renderer::vf::dynamic::part_index(
				2u),
			sge::renderer::vertex_count(
				_particle_count.get()),
			sge::renderer::resource_flags::none)),
	shader_(
		sge::shader::object_parameters(
			renderer_,
			*vertex_declaration_,
			sge::shader::vf_to_string<vf::format>(),
			fcppt::assign::make_container<sge::shader::variable_sequence>
				(sge::shader::variable(
					"maximum_distance",
					sge::shader::variable_type::constant,
					static_cast<sge::renderer::scalar>(
						_boundary.get().size()[0])))
				(sge::shader::variable(
					"camera_position",
					sge::shader::variable_type::uniform,
					sge::renderer::vector3()))
				(sge::shader::variable(
					"size_multiplier",
					sge::shader::variable_type::uniform,
					static_cast<sge::renderer::scalar>(
						1)))
				(sge::shader::variable(
					"mvp",
					sge::shader::variable_type::uniform,
					sge::shader::matrix(
						sge::renderer::matrix4(),
						sge::shader::matrix_flags::projection))),
			fcppt::assign::make_container<sge::shader::sampler_sequence>
				(sge::shader::sampler(
					"tex",
					sge::renderer::texture::create_planar_from_view(
						renderer_,
						_image_system.load(
							flakelib::media_path_from_string(FCPPT_TEXT("textures"))
								/ FCPPT_TEXT("flake.png"))->view(),
						sge::renderer::texture::mipmap::off(),
						sge::renderer::resource_flags_field(
							sge::renderer::resource_flags::none)))))
			.vertex_shader(
				flakelib::media_path_from_string(
					FCPPT_TEXT("shaders/flakes/vertex.glsl")))
			.fragment_shader(
				flakelib::media_path_from_string(
					FCPPT_TEXT("shaders/flakes/fragment.glsl")))
			.name(
				FCPPT_TEXT("flakes shader"))),
	cl_positions_buffer_(),
	cl_starting_positions_buffer_(),
	cl_point_sizes_buffer_(),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/volume/flakes.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	advect_kernel_(
		program_,
		sge::opencl::kernel::name(
			"advect")),
	planar_ptrs_(),
	cl_images_(),
	current_texture_(
		0)
{
	FCPPT_ASSERT_PRE(
		_boundary.get().size()[0] == _boundary.get().size()[1] &&
		_boundary.get().size()[1] == _boundary.get().size()[2]);

	this->generate_snow_textures(
		_snow_texture_size);

	this->generate_particles(
		_particle_count,
		_particle_minimum_size,
		_particle_maximum_size,
		volume::grid_size(
			_boundary.get().size()));

	cl_positions_buffer_.take(
		fcppt::make_unique_ptr<sge::opencl::memory_object::buffer>(
			fcppt::ref(
				command_queue_.context()),
			fcppt::ref(
				*positions_buffer_),
			sge::opencl::memory_object::renderer_buffer_lock_mode::read_write));

	cl_starting_positions_buffer_.take(
		fcppt::make_unique_ptr<sge::opencl::memory_object::buffer>(
			fcppt::ref(
				command_queue_.context()),
			fcppt::ref(
				*starting_positions_buffer_),
			sge::opencl::memory_object::renderer_buffer_lock_mode::read_write));

	cl_point_sizes_buffer_.take(
		fcppt::make_unique_ptr<sge::opencl::memory_object::buffer>(
			fcppt::ref(
				command_queue_.context()),
			fcppt::ref(
				*point_sizes_buffer_),
			sge::opencl::memory_object::renderer_buffer_lock_mode::read_write));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		static_cast<cl_int>(
			_boundary.get().size()[0]));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_boundary.get().buffer());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		static_cast<cl_int>(
			_particle_count.get()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		*cl_positions_buffer_);

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		*cl_starting_positions_buffer_);

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			5),
		*cl_point_sizes_buffer_);
}

void
flakelib::volume::flakes::object::update(
	flakelib::duration const &_dt,
	buffer::volume_view<cl_float4> const &_velocity)
{
	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		cl_positions_buffer_.get());
	mem_objects.push_back(
		cl_starting_positions_buffer_.get());
	mem_objects.push_back(
		cl_point_sizes_buffer_.get());
	mem_objects.push_back(
		&cl_images_[0]);
	mem_objects.push_back(
		&cl_images_[1]);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			6),
		_velocity.buffer());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			7),
		static_cast<cl_float>(
			_dt.count()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			8),
		cl_images_[current_texture_]);

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			9),
		cl_images_[(current_texture_+1u) % 2u]);

	current_texture_ =
		(current_texture_ + 1u) % 2u;

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		advect_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(positions_buffer_->size().get()).container());
}

void
flakelib::volume::flakes::object::render()
{
	sge::shader::scoped scoped_shader(
		shader_,
		sge::shader::activate_everything());

	shader_.update_uniform(
		"camera_position",
		camera_.gizmo().position());

	shader_.update_uniform(
		"mvp",
		sge::shader::matrix(
			camera_.mvp(),
			sge::shader::matrix_flags::projection));

	sge::renderer::scoped_vertex_buffer scoped_positions_vb(
		renderer_,
		*positions_buffer_);

	sge::renderer::scoped_vertex_buffer scoped_starting_positions_vb(
		renderer_,
		*starting_positions_buffer_);

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
			0),
		sge::renderer::vertex_count(
			positions_buffer_->size()),
		sge::renderer::nonindexed_primitive_type::point);
}

void
flakelib::volume::flakes::object::size_multiplier(
	sge::renderer::scalar const _size_multiplier)
{
	sge::shader::update_single_uniform(
		shader_,
		"size_multiplier",
		_size_multiplier);
}

sge::renderer::texture::planar_ptr const
flakelib::volume::flakes::object::current_snow_texture()
{
	return
//		planar_ptrs_[current_texture_];
		planar_ptrs_[0];
}

flakelib::volume::flakes::object::~object()
{
}

void
flakelib::volume::flakes::object::generate_snow_textures(
	flakes::snow_texture_size const &_snow_texture_size)
{
	for(std::size_t i = 0; i < 2; ++i)
	{
		planar_ptrs_[i] =
			renderer_.create_planar_texture(
				sge::renderer::texture::planar_parameters(
					_snow_texture_size.get(),
					sge::image::color::format::r32f,
					sge::renderer::texture::mipmap::off(),
					sge::renderer::resource_flags_field(
						sge::renderer::resource_flags::none),
					sge::renderer::texture::capabilities_field::null()));

		{
			sge::renderer::texture::scoped_planar_lock slock(
				*planar_ptrs_[i],
				sge::renderer::lock_mode::writeonly);

			sge::image2d::algorithm::fill(
				slock.value(),
				sge::image::colors::black());
		}

		fcppt::container::ptr::replace_unique_ptr(
			cl_images_,
			i,
			fcppt::make_unique_ptr<sge::opencl::memory_object::image::planar>(
				fcppt::ref(
					command_queue_.context()),
				sge::opencl::memory_object::flags_field(
					sge::opencl::memory_object::flags::read) |
					sge::opencl::memory_object::flags::write,
				fcppt::ref(
					*planar_ptrs_[i])));
	}
}

void
flakelib::volume::flakes::object::generate_particles(
	flakes::particle_count const &_particle_count,
	flakes::particle_minimum_size const &_particle_minimum_size,
	flakes::particle_maximum_size const &_particle_maximum_size,
	volume::grid_size const &_grid_size)
{
	sge::renderer::scoped_vertex_lock const positions_vblock(
		*positions_buffer_,
		sge::renderer::lock_mode::writeonly);

	sge::renderer::scoped_vertex_lock const starting_positions_vblock(
		*starting_positions_buffer_,
		sge::renderer::lock_mode::writeonly);

	sge::renderer::scoped_vertex_lock const point_sizes_vblock(
		*point_sizes_buffer_,
		sge::renderer::lock_mode::writeonly);

	sge::renderer::vf::view<vf::position_part> const positions(
		positions_vblock.value());

	sge::renderer::vf::view<vf::starting_position_part> const starting_positions(
		starting_positions_vblock.value());

	sge::renderer::vf::view<vf::point_size_part> const point_sizes(
		point_sizes_vblock.value());

	sge::renderer::vf::view<vf::position_part>::iterator positions_it(
		positions.begin());

	sge::renderer::vf::view<vf::starting_position_part>::iterator starting_positions_it(
		starting_positions.begin());

	sge::renderer::vf::view<vf::point_size_part>::iterator point_sizes_it(
		point_sizes.begin());

	fcppt::random::default_generator number_generator(
		static_cast<fcppt::random::default_generator::result_type>(
			fcppt::chrono::high_resolution_clock::now().time_since_epoch().count()));

	fcppt::random::uniform<sge::renderer::scalar,fcppt::random::default_generator &> size_rng_(
		fcppt::random::make_inclusive_range<cl_float>(
			_particle_minimum_size.get(),
			_particle_maximum_size.get()),
		number_generator);

	fcppt::random::uniform<sge::renderer::scalar,fcppt::random::default_generator &>
		x_rng_(
			fcppt::random::make_inclusive_range<sge::renderer::scalar>(
				1.0f,
				static_cast<sge::renderer::scalar>(
					_grid_size.get()[0]/2-1)),
			number_generator),
		y_rng_(
			fcppt::random::make_inclusive_range<sge::renderer::scalar>(
				static_cast<sge::renderer::scalar>(
					_grid_size.get()[1]/5),
				static_cast<sge::renderer::scalar>(
					_grid_size.get()[1]/2+10)),
			number_generator),
		second_y_rng_(
			fcppt::random::make_inclusive_range<sge::renderer::scalar>(
				static_cast<sge::renderer::scalar>(
					3),
				static_cast<sge::renderer::scalar>(
					_grid_size.get()[1]/2+10)),
			number_generator),
		z_rng_(
			fcppt::random::make_inclusive_range<cl_float>(
				1.0f,
				static_cast<sge::renderer::scalar>(
					_grid_size.get()[2])-1),
			number_generator);

	for(
		flakes::particle_count i(
			0);
		i != _particle_count;
		++i)
	{
		sge::renderer::vector4 starting_position =
			sge::renderer::vector4(
				x_rng_(),
				y_rng_(),
				z_rng_(),
				1.0f);

		if(starting_position[0] < _grid_size.get()[0] / 10)
		{
			starting_position[1] =
				second_y_rng_();
		}

		(*point_sizes_it++).set<vf::point_size>(
			vf::point_size::packed_type(
				size_rng_()));

		(*positions_it++).set<vf::position>(
			starting_position);

		(*starting_positions_it++).set<vf::starting_position>(
			starting_position);
	}
}
