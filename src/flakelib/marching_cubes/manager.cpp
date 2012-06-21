#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/marching_cubes/manager.hpp>
#include <flakelib/marching_cubes/num_vert_table.hpp>
#include <flakelib/marching_cubes/triangle_table.hpp>
#include <flakelib/marching_cubes/vf/format.hpp>
#include <flakelib/volume/gradient.hpp>
#include <flakelib/volume/unique_float4_buffer_lock.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/command_queue/scoped_buffer_mapping.hpp>
#include <sge/opencl/context/object.hpp>
#include <sge/opencl/memory_object/create_image_format.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <fcppt/insert_to_std_string.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/error.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/config/external_begin.hpp>
#include <cstdlib>
#include <iostream>
#include <fcppt/config/external_end.hpp>


namespace
{
sge::opencl::size_type const nthreads =
	32u;
}

flakelib::marching_cubes::manager::manager(
	sge::renderer::device &_renderer,
	flakelib::scan::object &_scan,
	flakelib::volume::gradient &_gradient,
	flakelib::cl::program_context const &_program_context,
	flakelib::buffer_pool::object &_buffer_pool)
:
	command_queue_(
		_program_context.command_queue()),
	renderer_(
		_renderer),
	scan_(
		_scan),
	vertex_declaration_(
		renderer_.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format
			<
				flakelib::marching_cubes::vf::format
			>())),
	children_(),
	table_format_(
		sge::opencl::memory_object::create_image_format(
			CL_R,
			CL_UNSIGNED_INT8)),
	triangle_table_error_code_(
		0),
	num_vert_table_error_code_(
		0),
	triangle_table_(
		clCreateImage2D(
			_program_context.command_queue().context().impl(),
			CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			&table_format_,
			16,
			256,
			0,
			&triangle_table,
			&triangle_table_error_code_)),
	num_vert_table_(
		clCreateImage2D(
			_program_context.command_queue().context().impl(),
			CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			&table_format_,
			256,
			1,
			0,
			&num_vert_table,
			&num_vert_table_error_code_)),
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/marching_cubes/main.cl")),
		_program_context.compiler_flags() +
		flakelib::cl::compiler_flags(
			std::string(
				" -DNTHREADS=")+
			fcppt::insert_to_std_string(
				nthreads))),
	classify_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"classify_voxels"))),
	compact_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"compact_voxels"))),
	generate_triangles_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"generate_triangles"))),
	gradient_(
		_gradient),
	debug_buffer_(
		fcppt::make_unique_ptr<linear_uint_lock>(
			fcppt::ref(
				_buffer_pool),
			sge::opencl::dim1(
				sizeof(
					cl_uint))))
{
	FCPPT_ASSERT_PRE(
		triangle_table_error_code_ == CL_SUCCESS);

	FCPPT_ASSERT_PRE(
		num_vert_table_error_code_ == CL_SUCCESS);

	classify_kernel_->buffer_argument(
		"debug_buffer",
		debug_buffer_->value().buffer());

	classify_kernel_->raw_memory_argument(
		"numVertsTex",
		num_vert_table_);

	generate_triangles_kernel_->buffer_argument(
		"debug_buffer",
		debug_buffer_->value().buffer());

	generate_triangles_kernel_->raw_memory_argument(
		"numVertsTex",
		num_vert_table_);

	generate_triangles_kernel_->raw_memory_argument(
		"triTex",
		triangle_table_);

	compact_kernel_->buffer_argument(
		"debug_buffer",
		debug_buffer_->value().buffer());

	{
		sge::opencl::command_queue::scoped_buffer_mapping buffer_mapping(
			generate_triangles_kernel_->command_queue(),
			debug_buffer_->value().buffer(),
			sge::opencl::command_queue::map_flags::write,
			sge::opencl::memory_object::byte_offset(
				0u),
			debug_buffer_->value().buffer().byte_size(),
			sge::opencl::event::sequence());

		*static_cast<cl_uint *>(buffer_mapping.ptr()) = 0;
	}

	this->check_debug_buffer("initial check");
}

void
flakelib::marching_cubes::manager::render(
	sge::renderer::context::object &_context)
{
	sge::renderer::state::scoped scoped_state(
		_context,
		sge::renderer::state::list
			(sge::renderer::state::depth_func::less)
			(sge::renderer::state::cull_mode::off));

	sge::renderer::scoped_vertex_declaration scoped_vertex_declaration(
		_context,
		*vertex_declaration_);

	for(
		child_sequence::iterator it =
			children_.begin();
		it != children_.end();
		++it)
		it->render(
			_context);
}

sge::renderer::vertex_declaration &
flakelib::marching_cubes::manager::vertex_declaration()
{
	return
		*vertex_declaration_;
}

void
flakelib::marching_cubes::manager::classify_voxels(
	flakelib::marching_cubes::density_view const &_density,
	flakelib::marching_cubes::vertices_for_voxel_view const &_vertices_for_voxel,
	flakelib::marching_cubes::voxel_occupation_view const &_voxel_occupation,
	flakelib::marching_cubes::iso_level const &_iso_level,
	flakelib::marching_cubes::grid_size const &_grid_size,
	flakelib::marching_cubes::grid_size_mask const &_grid_size_mask,
	flakelib::marching_cubes::grid_size_shift const &_grid_size_shift)
{
	classify_kernel_->buffer_argument(
		"density",
		_density.get().buffer());

	classify_kernel_->buffer_argument(
		"vertices_for_voxel",
		_vertices_for_voxel.get().buffer());

	classify_kernel_->numerical_argument(
		"vertices_for_voxel_size",
		static_cast<cl_uint>(
			_vertices_for_voxel.get().size().content()));

	classify_kernel_->buffer_argument(
		"voxel_occupation",
		_voxel_occupation.get().buffer());

	classify_kernel_->numerical_argument(
		"voxel_occupation_size",
		static_cast<cl_uint>(
			_voxel_occupation.get().size().content()));

	classify_kernel_->vector_argument(
		"grid_size",
		_grid_size.get());

	classify_kernel_->vector_argument(
		"grid_size_shift",
		_grid_size_shift.get());

	classify_kernel_->vector_argument(
		"grid_size_mask",
		_grid_size_mask.get());

	classify_kernel_->numerical_argument(
		"iso_value",
		_iso_level.get());

	classify_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim1(
			sge::opencl::dim1(
				_density.get().size().content())));

	this->check_debug_buffer(
		"classifyVoxels");
}

flakelib::scan::object &
flakelib::marching_cubes::manager::scan() const
{
	return
		scan_;
}

sge::renderer::device &
flakelib::marching_cubes::manager::renderer() const
{
	return
		renderer_;
}

sge::renderer::vertex_declaration const &
flakelib::marching_cubes::manager::vertex_declaration() const
{
	return
		*vertex_declaration_;
}

flakelib::marching_cubes::manager::~manager()
{
}

void
flakelib::marching_cubes::manager::add_child(
	flakelib::marching_cubes::object &_child)
{
	children_.push_back(
		_child);
}

void
flakelib::marching_cubes::manager::compact_voxels(
	flakelib::marching_cubes::voxel_occupation_view const &_voxel_occupation,
	flakelib::marching_cubes::summed_voxel_occupation_view const &_summed_voxel_occupation,
	flakelib::marching_cubes::compacted_voxel_occupation_view const &_compacted_voxel_occupation)
{
	compact_kernel_->buffer_argument(
		"voxel_occupation",
		_voxel_occupation.get().buffer());

	compact_kernel_->numerical_argument(
		"voxel_occupation_size",
		static_cast<cl_uint>(
			_voxel_occupation.get().size().content()));

	compact_kernel_->buffer_argument(
		"summed_voxel_occupation",
		_summed_voxel_occupation.get().buffer());

	compact_kernel_->numerical_argument(
		"summed_voxel_occupation_size",
		static_cast<cl_uint>(
			_summed_voxel_occupation.get().size().content()));

	compact_kernel_->buffer_argument(
		"compacted_voxel_occupation",
		_compacted_voxel_occupation.get().buffer());

	compact_kernel_->numerical_argument(
		"compacted_voxel_occupation_size",
		static_cast<cl_uint>(
			_compacted_voxel_occupation.get().size().content()));

	compact_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim1(
			sge::opencl::dim1(
				_voxel_occupation.get().size().content())));

	this->check_debug_buffer(
		"compactVoxels");
}

void
flakelib::marching_cubes::manager::generate_triangles(
	flakelib::marching_cubes::normals_buffer const &_normals_buffer,
	flakelib::marching_cubes::positions_buffer const &_positions_buffer,
	flakelib::marching_cubes::compacted_voxel_occupation_view const &_compacted_voxel_occupation_view,
	flakelib::marching_cubes::summed_vertices_for_voxel_view const &_summed_vertices_for_voxel_view,
	flakelib::marching_cubes::density_view const &_density_view,
	flakelib::marching_cubes::grid_size const &_grid_size,
	flakelib::marching_cubes::grid_size_mask const &_grid_size_mask,
	flakelib::marching_cubes::grid_size_shift const &_grid_size_shift,
	flakelib::marching_cubes::iso_level const &_iso_level,
	flakelib::marching_cubes::active_voxels const &_active_voxels,
	flakelib::marching_cubes::vertex_count const &_vertex_count)
{
	generate_triangles_kernel_->buffer_argument(
		"positions",
		_positions_buffer.get());

	generate_triangles_kernel_->numerical_argument(
		"positions_size",
		static_cast<cl_uint>(
			_positions_buffer.get().byte_size().get() / (4u*sizeof(cl_float))));

	generate_triangles_kernel_->buffer_argument(
		"normals",
		_normals_buffer.get());

	generate_triangles_kernel_->numerical_argument(
		"normals_size",
		static_cast<cl_uint>(
			_positions_buffer.get().byte_size().get() / (4u*sizeof(cl_float))));

	flakelib::volume::unique_float4_buffer_lock gradient(
		gradient_.update(
			_density_view.get()));

	generate_triangles_kernel_->buffer_argument(
		"gradient",
		gradient->value().buffer());

	generate_triangles_kernel_->buffer_argument(
		"compacted_voxel_occupation",
		_compacted_voxel_occupation_view.get().buffer());

	generate_triangles_kernel_->numerical_argument(
		"compacted_voxel_occupation_size",
		static_cast<cl_uint>(
			_compacted_voxel_occupation_view.get().size().content()));

	generate_triangles_kernel_->buffer_argument(
		"summed_vertices_for_voxel",
		_summed_vertices_for_voxel_view.get().buffer());

	generate_triangles_kernel_->numerical_argument(
		"summed_vertices_for_voxel_size",
		static_cast<cl_uint>(
			_summed_vertices_for_voxel_view.get().size().content()));

	generate_triangles_kernel_->buffer_argument(
		"volume",
		_density_view.get().buffer());

	generate_triangles_kernel_->vector_argument(
		"grid_size",
		_grid_size.get());

	generate_triangles_kernel_->vector_argument(
		"grid_size_shift",
		_grid_size_shift.get());

	generate_triangles_kernel_->vector_argument(
		"grid_size_mask",
		_grid_size_mask.get());

	generate_triangles_kernel_->numerical_argument(
		"iso_value",
		_iso_level.get());

	generate_triangles_kernel_->numerical_argument(
		"active_voxels",
		_active_voxels.get());

	generate_triangles_kernel_->numerical_argument(
		"max_verts",
		static_cast<cl_uint>(
			_vertex_count.get()));

	generate_triangles_kernel_->enqueue(
		sge::opencl::command_queue::global_dim1(
			sge::opencl::dim1(
				static_cast<sge::opencl::size_type>(
					_grid_size.get().w() * _grid_size.get().h() * _grid_size.get().d()))),
		sge::opencl::command_queue::local_dim1(
			sge::opencl::dim1(
				nthreads)));

	this->check_debug_buffer(
		"generateTriangles");
}

void
flakelib::marching_cubes::manager::check_debug_buffer(
	std::string const &_operation)
{
	sge::opencl::command_queue::scoped_buffer_mapping buffer_mapping(
		generate_triangles_kernel_->command_queue(),
		debug_buffer_->value().buffer(),
		sge::opencl::command_queue::map_flags::read,
		sge::opencl::memory_object::byte_offset(
			0u),
		debug_buffer_->value().buffer().byte_size(),
		sge::opencl::event::sequence());

	if(*static_cast<cl_uint *>(
			buffer_mapping.ptr()))
	{
		std::cerr << _operation << ": Debug buffer contains: " << *static_cast<cl_uint *>(
			buffer_mapping.ptr()) << "!!!!\n";
		std::exit(0);
	}
	else
	{
		std::cerr << _operation << " everything ok\n";
		/*
		std::cerr << "Debug buffer contains: " << *static_cast<cl_uint *>(
			buffer_mapping.ptr()) << "!!!!\n";
			*/
	}
}
