#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/marching_cubes/manager.hpp>
#include <flakelib/marching_cubes/num_vert_table.hpp>
#include <flakelib/marching_cubes/triangle_table.hpp>
#include <flakelib/marching_cubes/vf/format.hpp>
#include <flakelib/volume/gradient.hpp>
#include <flakelib/volume/unique_float4_buffer_lock.hpp>
#include <sge/opencl/command_queue/object.hpp>
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
#include <fcppt/text.hpp>
#include <fcppt/assert/error.hpp>
#include <fcppt/assert/pre.hpp>


flakelib::marching_cubes::manager::manager(
	sge::renderer::device &_renderer,
	flakelib::volume::gradient &_gradient,
	flakelib::cl::program_context const &_program_context)
:
	command_queue_(
		_program_context.command_queue()),
	renderer_(
		_renderer),
	vertex_declaration_(
		renderer_.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format
			<
				flakelib::marching_cubes::vf::format
			>())),
	scan_(
		_program_context),
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
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/marching_cubes/main.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_program_context.compiler_flags().get()))),
	classifyVoxelKernel(
		program_,
		sge::opencl::kernel::name(
			"classifyVoxel")),
	compactVoxelsKernel(
		program_,
		sge::opencl::kernel::name(
			"compactVoxels")),
	generateTriangles2Kernel(
		program_,
		sge::opencl::kernel::name(
			"generateTriangles2")),
	gradient_(
		_gradient)
{
	FCPPT_ASSERT_PRE(
		triangle_table_error_code_ == CL_SUCCESS);

	FCPPT_ASSERT_PRE(
		num_vert_table_error_code_ == CL_SUCCESS);
}

void
flakelib::marching_cubes::manager::render()
{
	sge::renderer::state::scoped scoped_state(
		renderer_,
		sge::renderer::state::list
			(sge::renderer::state::depth_func::less)
			(sge::renderer::state::cull_mode::off));

	sge::renderer::scoped_vertex_declaration scoped_vertex_declaration(
		renderer_,
		*vertex_declaration_);

	for(
		child_sequence::iterator it =
			children_.begin();
		it != children_.end();
		++it)
		it->render();
}

sge::renderer::vertex_declaration &
flakelib::marching_cubes::manager::vertex_declaration()
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
flakelib::marching_cubes::manager::launch_classifyVoxel(
	sge::opencl::memory_object::dim3 grid,
	sge::opencl::memory_object::dim3 threads,
	cl_mem voxelVerts,
	cl_mem voxelOccupied,
	cl_mem volume,
	cl_uint gridSize[4],
	cl_uint gridSizeShift[4],
	cl_uint gridSizeMask[4],
	uint numVoxels,
	cl_float voxelSize[4],
	float isoValue)
{
	cl_int ciErrNum;
	ciErrNum = clSetKernelArg(classifyVoxelKernel.impl(), 0, sizeof(cl_mem), &voxelVerts);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(classifyVoxelKernel.impl(), 1, sizeof(cl_mem), &voxelOccupied);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(classifyVoxelKernel.impl(), 2, sizeof(cl_mem), &volume);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(classifyVoxelKernel.impl(), 3, 4 * sizeof(cl_uint), gridSize);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(classifyVoxelKernel.impl(), 4, 4 * sizeof(cl_uint), gridSizeShift);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(classifyVoxelKernel.impl(), 5, 4 * sizeof(cl_uint), gridSizeMask);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(classifyVoxelKernel.impl(), 6, sizeof(uint), &numVoxels);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(classifyVoxelKernel.impl(), 7, 4 * sizeof(cl_float), voxelSize);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(classifyVoxelKernel.impl(), 8, sizeof(float), &isoValue);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(classifyVoxelKernel.impl(), 9, sizeof(cl_mem), &num_vert_table_);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);

	grid.w() *= threads.w();
	ciErrNum = clEnqueueNDRangeKernel(command_queue_.impl(), classifyVoxelKernel.impl(), 1, NULL, (size_t*)grid.data(), (size_t*) threads.data(), 0, 0, 0);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
}

void
flakelib::marching_cubes::manager::launch_compactVoxels(
	sge::opencl::memory_object::dim3 grid,
	sge::opencl::memory_object::dim3 threads,
	cl_mem compVoxelArray,
	cl_mem voxelOccupied,
	cl_mem voxelOccupiedScan,
	uint numVoxels)
{
	cl_int ciErrNum;
	ciErrNum = clSetKernelArg(compactVoxelsKernel.impl(), 0, sizeof(cl_mem), &compVoxelArray);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(compactVoxelsKernel.impl(), 1, sizeof(cl_mem), &voxelOccupied);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(compactVoxelsKernel.impl(), 2, sizeof(cl_mem), &voxelOccupiedScan);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(compactVoxelsKernel.impl(), 3, sizeof(cl_uint), &numVoxels);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);

	grid.w() *= threads.w();
	ciErrNum = clEnqueueNDRangeKernel(command_queue_.impl(), compactVoxelsKernel.impl(), 1, NULL, (size_t*) grid.data(), (size_t*) threads.data(), 0, 0, 0);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
}

void
flakelib::marching_cubes::manager::launch_generateTriangles2(
	flakelib::volume::float_view const &_view,
	sge::opencl::memory_object::dim3 grid,
	sge::opencl::memory_object::dim3 threads,
	cl_mem pos,
	cl_mem norm,
	cl_mem compactedVoxelArray,
	cl_mem numVertsScanned,
	cl_mem volume,
	cl_uint gridSize[4],
	cl_uint gridSizeShift[4],
	cl_uint gridSizeMask[4],
	cl_float voxelSize[4],
	float isoValue,
	uint activeVoxels,
	sge::renderer::size_type const _vertex_count)
{
	flakelib::volume::unique_float4_buffer_lock gradient(
		gradient_.update(
			_view));

	cl_int ciErrNum;
	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 0, sizeof(cl_mem), &pos);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 1, sizeof(cl_mem), &norm);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 2, sizeof(cl_mem), &compactedVoxelArray);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 3, sizeof(cl_mem), &numVertsScanned);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 4, sizeof(cl_mem), &volume);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 5, 4 * sizeof(cl_uint), gridSize);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 6, 4 * sizeof(cl_uint), gridSizeShift);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 7, 4 * sizeof(cl_uint), gridSizeMask);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 8, 4 * sizeof(cl_float), voxelSize);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 9, sizeof(float), &isoValue);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 10, sizeof(uint), &activeVoxels);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	cl_uint const cl_vertex_count =
	static_cast<cl_uint>(
		_vertex_count);
	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 11, sizeof(uint), &cl_vertex_count);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);

	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 12, sizeof(cl_mem), &num_vert_table_);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 13, sizeof(cl_mem), &triangle_table_);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
	cl_mem gradient_buffer = gradient->value().buffer().impl();
	ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 14, sizeof(cl_mem), &gradient_buffer);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);

	grid.w() *= threads.w();
	ciErrNum = clEnqueueNDRangeKernel(command_queue_.impl(), generateTriangles2Kernel.impl(), 1, NULL, (size_t*) grid.data(), (size_t*) threads.data(), 0, 0, 0);
	FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
}
