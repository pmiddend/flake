#include <fcppt/math/dim/comparison.hpp>
#include <sge/renderer/texture/address_mode2.hpp>
#include <sge/renderer/texture/set_address_mode2.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/volume/density/visual.hpp>
#include <flakelib/volume/density/cube_vf/format.hpp>
#include <flakelib/volume/density/cube_vf/position.hpp>
#include <flakelib/volume/density/cube_vf/vertex_view.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/planar_parameters.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/vf/iterator.hpp>
#include <sge/renderer/vf/vertex.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <algorithm>
#include <cmath>
#include <fcppt/config/external_end.hpp>

// DEBUG
#include <sge/renderer/texture/const_scoped_planar_lock.hpp>
#include <sge/image2d/save_from_view.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <sge/image2d/l8.hpp>
#include <sge/image2d/dim.hpp>
#include <sge/image2d/view/object.hpp>
#include <sge/image2d/algorithm/copy_and_convert.hpp>
#include <sge/image2d/view/to_const.hpp>
#include <sge/image2d/view/to_const.hpp>
#include <sge/image/store.hpp>

namespace
{
sge::renderer::dim2 const
planar_size_from_volume_size(
	sge::opencl::memory_object::dim3 const &_volume_size)
{
	sge::renderer::dim2::value_type const edge_length =
		static_cast<sge::renderer::dim2::value_type>(
			std::ceil(
				std::sqrt(
					static_cast<double>(
						_volume_size.content()))));
	return
		sge::renderer::dim2(
			edge_length,
			edge_length);
}
}

flakelib::volume::density::visual::visual(
	sge::renderer::device &_renderer,
	sge::image2d::system &_image_system,
	sge::opencl::command_queue::object &_command_queue,
	flakelib::build_options const &_build_options,
	density::grid_size const &_grid_size,
	boundary::view const &_boundary)
:
	renderer_(
		_renderer),
	command_queue_(
		_command_queue),
	image_system_(
		_image_system),
	boundary_(
		_boundary),
	boundary_texture_(
		_renderer.create_planar_texture(
			sge::renderer::texture::planar_parameters(
				planar_size_from_volume_size(
					_grid_size.get()),
				sge::image::color::format::r32f,
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field(
					sge::renderer::resource_flags::readable),
				sge::renderer::texture::capabilities_field::null()))),
	texture_(
		_renderer.create_planar_texture(
			sge::renderer::texture::planar_parameters(
				planar_size_from_volume_size(
					_grid_size.get()),
				sge::image::color::format::r32f,
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field(
					sge::renderer::resource_flags::readable),
				sge::renderer::texture::capabilities_field::null()))),
	boundary_cl_texture_(
		_command_queue.context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		*boundary_texture_),
	cl_texture_(
		_command_queue.context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		*texture_),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/volume/visual.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	volume_image_to_planar_texture_kernel_(
		program_,
		sge::opencl::kernel::name(
			"volume_image_to_planar_texture")),
	vd_(
		renderer_.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<cube_vf::format>())),
	vb_(
		renderer_.create_vertex_buffer(
			*vd_,
			sge::renderer::vf::dynamic::part_index(
				0u),
			sge::renderer::vertex_count(
				2u * 3u * 6u),
			sge::renderer::resource_flags::none)),
	shader_(
		sge::shader::object_parameters(
			renderer_,
			*vd_,
			sge::shader::vf_to_string<cube_vf::format>(),
			fcppt::assign::make_container<sge::shader::variable_sequence>
				(sge::shader::variable(
					"mvp",
					sge::shader::variable_type::uniform,
					sge::shader::matrix(
						sge::renderer::matrix4::identity(),
						sge::shader::matrix_flags::projection)))
				(sge::shader::variable(
					"cube_position",
					sge::shader::variable_type::uniform,
					sge::renderer::vector3::null()))
				// FIXME: The edge length might be bigger due to grid scaling
				(sge::shader::variable(
					"cube_edge_length",
					sge::shader::variable_type::constant,
					static_cast<sge::renderer::scalar>(
						_grid_size.get()[0])))
				(sge::shader::variable(
					"slice_width",
					sge::shader::variable_type::uniform,
					static_cast<sge::renderer::glsl::uniform::int_type>(
						_grid_size.get()[0])))
				(sge::shader::variable(
					"elements_per_row",
					sge::shader::variable_type::uniform,
					static_cast<sge::renderer::glsl::uniform::int_type>(
						texture_->size()[0] / _grid_size.get()[0])))
				(sge::shader::variable(
					"camera",
					sge::shader::variable_type::uniform,
					sge::renderer::vector3())),
			fcppt::assign::make_container<sge::shader::sampler_sequence>
				(sge::shader::sampler(
					"smoke_texture",
					texture_))
				(sge::shader::sampler(
					"boundary_texture",
					boundary_texture_)))
				.vertex_shader(
					flakelib::media_path_from_string(
						FCPPT_TEXT("shaders/cube/vertex.glsl")))
				.fragment_shader(
					flakelib::media_path_from_string(
						FCPPT_TEXT("shaders/cube/fragment.glsl")))
				.name(
					FCPPT_TEXT("cube shader")))
{
	/*
	this->volume_image_to_planar_texture(
		_boundary.get(),
		boundary_cl_texture_);

	this->save_texture_to_file(
		*boundary_texture_,
		fcppt::filesystem::path(FCPPT_TEXT("/tmp/boundary.png")));
		*/

	sge::renderer::scoped_vertex_lock const vblock(
		*vb_,
		sge::renderer::lock_mode::writeonly);

	cube_vf::vertex_view const vertices(
		vblock.value());

	cube_vf::vertex_view::iterator vb_it(
		vertices.begin());

	typedef
	cube_vf::position::packed_type
	position_vector;

	// Here be dragons...
	int a = -1;
	int b = 1;

	for (unsigned side = 0; side < 6; ++side)
	{
		for (unsigned tri = 0; tri < 2; ++tri)
		{
			for (unsigned i = 0; i < 3; ++i)
			{
				unsigned vert = (tri == 0) ? i : 2 - i;
				unsigned x = side % 3;
				int y = (vert == 0) ? a : b;
				int z = (vert == 2) ? b : a;
				if (
					((side % 2 == 0) && tri == 1) ||
					((side % 2 == 1) && tri == 0))
				{
					std::swap(y,z);
				}
				position_vector res(0,0,0);
				res[x] = static_cast<position_vector::value_type>(
					(side > x) ? b : a);
				res[x == 0 ? 1 : 0] = static_cast<position_vector::value_type>(
					y);
				res[x == 2 ? 1 : 2] = static_cast<position_vector::value_type>(
					z);

				(*vb_it++).set<cube_vf::position>(
					// FIXME: grid scaling
					position_vector(
						static_cast<sge::renderer::scalar>(
							_grid_size.get()[0]),
						static_cast<sge::renderer::scalar>(
							_grid_size.get()[1]),
						static_cast<sge::renderer::scalar>(
							_grid_size.get()[2]))/2.0f *
					(position_vector(1,1,1) + res));
			}
		}
	}
}

void
flakelib::volume::density::visual::update(
	flakelib::buffer::volume_view<cl_float> const &_view)
{
	this->volume_image_to_planar_texture(
		_view,
		cl_texture_);

	this->volume_image_to_planar_texture(
		boundary_.get(),
		boundary_cl_texture_);

	this->save_texture_to_file(
		*boundary_texture_,
		fcppt::filesystem::path(FCPPT_TEXT("/tmp/boundary.png")));
}

void
flakelib::volume::density::visual::render(
	sge::renderer::vector3 const &_camera,
	sge::renderer::matrix4 const &_mvp)
{
	/*
	sge::renderer::texture::set_address_mode2(
		renderer_,
		sge::renderer::texture::stage(
			0),
		sge::renderer::texture::address_mode2(
			sge::renderer::texture::address_mode::clamp));
			*/

	sge::shader::scoped scoped_shader(
		shader_,
		sge::shader::activate_everything());

	shader_.update_uniform(
		"camera",
		_camera);

	shader_.update_uniform(
		"mvp",
		sge::shader::matrix(
			_mvp,
			sge::shader::matrix_flags::projection));

	sge::renderer::state::scoped scoped_state(
		renderer_,
		sge::renderer::state::list
			(sge::renderer::state::cull_mode::clockwise));

	sge::renderer::scoped_vertex_buffer const vb_context(
		renderer_,
		*vb_);

	renderer_.render_nonindexed(
		sge::renderer::first_vertex(
			0),
		sge::renderer::vertex_count(
			vb_->size()),
		sge::renderer::nonindexed_primitive_type::triangle);
}

flakelib::volume::density::visual::~visual()
{
}

void
flakelib::volume::density::visual::volume_image_to_planar_texture(
	flakelib::buffer::volume_view<cl_float> const &_volume_image,
	sge::opencl::memory_object::image::planar &_planar_texture)
{
	FCPPT_ASSERT_PRE(
		_volume_image.size()[0] == _volume_image.size()[1] &&
		_volume_image.size()[1] == _volume_image.size()[2]);

	sge::opencl::memory_object::base_ref_sequence mem_objects;

	mem_objects.push_back(
		&_planar_texture);

	sge::opencl::memory_object::scoped_objects scoped_objects(
		command_queue_,
		mem_objects);

	volume_image_to_planar_texture_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_volume_image.buffer());

	volume_image_to_planar_texture_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_planar_texture);

	volume_image_to_planar_texture_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		static_cast<cl_int>(
			_volume_image.size()[0]));

	volume_image_to_planar_texture_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_int>(
			_planar_texture.size()[0] / _volume_image.size()[0]));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		volume_image_to_planar_texture_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_volume_image.size()[0])
			(_volume_image.size()[1])
			(_volume_image.size()[2]).container());
}


void
flakelib::volume::density::visual::save_texture_to_file(
	sge::renderer::texture::planar &_texture,
	fcppt::filesystem::path const &_path)
{
	sge::renderer::texture::const_scoped_planar_lock slock(
		_texture);

	typedef sge::image2d::l8 store_type;

	store_type whole_store(
		fcppt::math::dim::structure_cast<sge::image2d::dim>(
			texture_->size()));

	sge::image2d::algorithm::copy_and_convert(
		slock.value(),
		sge::image2d::view::object(
			whole_store.wrapped_view()),
		sge::image::algorithm::may_overlap::no);

	sge::image2d::save_from_view(
		image_system_,
		sge::image2d::view::to_const(
			sge::image2d::view::object(
				whole_store.wrapped_view())),
		_path);
}
