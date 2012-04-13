#include <sge/renderer/state/cull_mode.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/volume_view.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/marching_cubes/num_vert_table.hpp>
#include <flakelib/marching_cubes/object.hpp>
#include <flakelib/marching_cubes/scan.hpp>
#include <flakelib/marching_cubes/triangle_table.hpp>
#include <flakelib/marching_cubes/vf/format.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/world.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/context/object.hpp>
#include <sge/opencl/memory_object/create_image_format.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/dynamic/make_part_index.hpp>
#include <sge/shader/activate_bare.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/optional.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/error.hpp>
#include <fcppt/assert/error_message.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/container/ptr/insert_unique_ptr_map.hpp>
#include <fcppt/filesystem/extension_without_dot.hpp>
#include <fcppt/filesystem/stem.hpp>
#include <fcppt/math/log2.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/operations.hpp>
#include <fcppt/config/external_end.hpp>

flakelib::marching_cubes::object::object(
	sge::renderer::device &_renderer,
	sge::camera::base &_camera,
	flakelib::cl::program_context const &_program_context,
	flakelib::volume::grid_size const &_grid_size)
:
	renderer_(
		_renderer),
	camera_(
		_camera),
	command_queue_(
		_program_context.command_queue()),
	grid_size_(
		_grid_size),
	vertex_declaration_(
		_renderer.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<vf::format>())),
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
					"world",
					sge::shader::variable_type::uniform,
					sge::shader::matrix(
						sge::renderer::matrix4::identity(),
						sge::shader::matrix_flags::none)))
				(sge::shader::variable(
					"sun_direction",
					sge::shader::variable_type::uniform,
					sge::renderer::vector3(
						0.8804509f,0.17609018f,0.440225453f))),
			sge::shader::sampler_sequence())
			.vertex_shader(
				flakelib::media_path_from_string(
					FCPPT_TEXT("shaders/marching_cubes/vertex.glsl")))
			.fragment_shader(
				flakelib::media_path_from_string(
					FCPPT_TEXT("shaders/marching_cubes/fragment.glsl")))
			.name(
				FCPPT_TEXT("Marching cubes"))),
	positions_buffer_(),
	normals_buffer_(),
	positions_buffer_cl_(),
	normals_buffer_cl_(),
	vertex_count_(
		0),
	scan_(
		_program_context),
	table_format_(
		sge::opencl::memory_object::create_image_format(
			CL_R,
			CL_UNSIGNED_INT8)),
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
	voxel_verts_(
		_program_context.command_queue().context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) |
		sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::byte_size(
			grid_size_.get().content() * sizeof(cl_uint))),
	voxel_verts_scan_(
		_program_context.command_queue().context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) |
		sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::byte_size(
			grid_size_.get().content() * sizeof(cl_uint))),
	voxel_occupied_(
		_program_context.command_queue().context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) |
		sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::byte_size(
			grid_size_.get().content() * sizeof(cl_uint))),
	voxel_occupied_scan_(
		_program_context.command_queue().context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) |
		sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::byte_size(
			grid_size_.get().content() * sizeof(cl_uint))),
	voxel_array_(
		_program_context.command_queue().context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) |
		sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::byte_size(
			grid_size_.get().content() * sizeof(cl_uint))),
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
			"generateTriangles2"))
{
	FCPPT_ASSERT_PRE(
		triangle_table_error_code_ == CL_SUCCESS);

	FCPPT_ASSERT_PRE(
		num_vert_table_error_code_ == CL_SUCCESS);
}

void
flakelib::marching_cubes::object::update(
	flakelib::volume::float_view const &_view)
{
	FCPPT_ASSERT_PRE(
		_view.size() == grid_size_.get());

	cl_uint const gridSizeLog2[4] =
	{
		fcppt::math::log2(
			static_cast<cl_uint>(
				grid_size_.get().w())),
		fcppt::math::log2(
			static_cast<cl_uint>(
				grid_size_.get().h())),
		fcppt::math::log2(
			static_cast<cl_uint>(
				grid_size_.get().d())),
		0u
	};

	std::cout << "Grid size log2: " << gridSizeLog2[0] << ", " << gridSizeLog2[1] << ", " << gridSizeLog2[2] << ", " << gridSizeLog2[3] << "\n";

	cl_uint gridSize[4] =
	{
		static_cast<cl_uint>(
			grid_size_.get().w()),
		static_cast<cl_uint>(
			grid_size_.get().h()),
		static_cast<cl_uint>(
			grid_size_.get().d()),
		0u
	};

	std::cout << "Grid size log2: " << gridSize[0] << ", " << gridSize[1] << ", " << gridSize[2] << ", " << gridSize[3] << "\n";

	cl_uint gridSizeMask[4] =
	{
		static_cast<cl_uint>(
			grid_size_.get().w()-1u),
		static_cast<cl_uint>(
			grid_size_.get().h()-1u),
		static_cast<cl_uint>(
			grid_size_.get().d()-1),
		0u
	};

	cl_uint gridSizeShift[4] =
	{
		0u,
		gridSizeLog2[0],
		gridSizeLog2[0]+gridSizeLog2[1],
		0u
	};

	cl_float voxelSize[4] =
	{
		2.0f / static_cast<cl_float>(gridSize[0]),
		2.0f / static_cast<cl_float>(gridSize[1]),
		2.0f / static_cast<cl_float>(gridSize[2]),
		0.0f
	};

	cl_uint const numVoxels =
		static_cast<cl_uint>(
			grid_size_.get().content());

	cl_float const isoValue = 0.2f;

	sge::opencl::memory_object::size_type const threads =
		128u;

	sge::opencl::memory_object::dim3 grid(
		grid_size_.get().content() / threads,
		1u,
		1u);

	// get around maximum grid size of 65535 in each dimension
	if(grid.w() > 65535u)
	{
		grid.h() = grid.w() / 32768u;
		grid.w() = 32768u;
	}


	// calculate number of vertices need per voxel
	launch_classifyVoxel(
		grid,
		sge::opencl::memory_object::dim3(
			threads,
			1u,
			1u),
		voxel_verts_.impl(),
		voxel_occupied_.impl(),
		_view.buffer().impl(),
		gridSize,
		gridSizeShift,
		gridSizeMask,
		numVoxels,
		voxelSize,
		isoValue);

	std::size_t const first_scan_result =
		scan_.scanExclusiveLarge(
			voxel_occupied_scan_.impl(),
			voxel_occupied_.impl(),
			1,
			numVoxels);

	std::cout << "First scan resulted in " << first_scan_result << "\n";

	cl_uint activeVoxels = 0;
	// read back values to calculate total number of non-empty voxels
	// since we are using an exclusive scan, the total is the last value of
	// the scan result plus the last value in the input array
	{
		cl_uint lastElement, lastScanElement;

		clEnqueueReadBuffer(
			command_queue_.impl(),
			voxel_occupied_.impl(),
			CL_TRUE,
			(numVoxels-1) * sizeof(cl_uint),
			sizeof(cl_uint),
			&lastElement,
			0,
			0,
			0);

		clEnqueueReadBuffer(
			command_queue_.impl(),
			voxel_occupied_scan_.impl(),
			CL_TRUE,
			(numVoxels-1) * sizeof(cl_uint),
			sizeof(cl_uint),
			&lastScanElement,
			0,
			0,
			0);

		activeVoxels = lastElement + lastScanElement;

		std::cout << "Last element: " << lastElement << ", lastScanElement: " << lastScanElement << "\n";
	}

    if (activeVoxels==0)
        return;

    // compact voxel index array
    launch_compactVoxels(
	    grid,
	    sge::opencl::memory_object::dim3(
		threads,
		1u,
		1u),
	    voxel_array_.impl(),
	    voxel_occupied_.impl(),
	    voxel_occupied_scan_.impl(),
	    numVoxels);

	std::size_t const second_scan_result =
		scan_.scanExclusiveLarge(
			voxel_verts_scan_.impl(),
			voxel_verts_.impl(),
			1,
			numVoxels);

	std::cout << "Second scan resulted in " << second_scan_result << "\n";


	// readback total number of vertices
	cl_uint totalVerts = 0;
	{
		cl_uint lastElement, lastScanElement;

		clEnqueueReadBuffer(
			command_queue_.impl(),
			voxel_verts_.impl(),
			CL_TRUE,
			(numVoxels-1) * sizeof(cl_uint),
			sizeof(cl_uint),
			&lastElement,
			0,
			0,
			0);

		clEnqueueReadBuffer(
			command_queue_.impl(),
			voxel_verts_scan_.impl(),
			CL_TRUE,
			(numVoxels-1) * sizeof(cl_uint),
			sizeof(cl_uint),
			&lastScanElement,
			0,
			0,
			0);

		totalVerts = lastElement + lastScanElement;
	}

	std::cout << "Got " << totalVerts << " vertices total\n";

	vertex_count_ = totalVerts;
	this->resize_gl_buffers();

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		positions_buffer_cl_.get());
	mem_objects.push_back(
		normals_buffer_cl_.get());

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	sge::opencl::memory_object::size_type const nthreads =
		32;

	sge::opencl::memory_object::dim3 grid2(
		static_cast<sge::opencl::memory_object::size_type>(
			(int)std::ceil(activeVoxels / (float)nthreads)),
		1u,
		1u);

	while(grid2.w() > 65535)
	{
		grid2.w()/=2;
		grid2.h()*=2;
	}


	launch_generateTriangles2(
		grid2,
		sge::opencl::memory_object::dim3(
		    nthreads,
		    1,
		    1),
		positions_buffer_cl_->impl(),
		normals_buffer_cl_->impl(),
		voxel_array_.impl(),
		voxel_verts_scan_.impl(),
		_view.buffer().impl(),
		gridSize,
		gridSizeShift,
		gridSizeMask,
		voxelSize,
		isoValue,
		activeVoxels);
}

void
flakelib::marching_cubes::object::render()
{
	if(!positions_buffer_)
		return;

	FCPPT_ASSERT_PRE(
		normals_buffer_);

	FCPPT_ASSERT_PRE(
		positions_buffer_->size() == normals_buffer_->size());

	sge::renderer::state::scoped scoped_state(
		renderer_,
		sge::renderer::state::list
			(sge::renderer::state::depth_func::less)
			(sge::renderer::state::cull_mode::off));

	sge::renderer::scoped_vertex_declaration scoped_vertex_declaration(
		renderer_,
		*vertex_declaration_);

	sge::shader::scoped scoped_shader(
		shader_,
		sge::shader::activate_bare());

	shader_.update_uniform(
		"mvp",
		sge::shader::matrix(
			sge::camera::matrix_conversion::world_projection(
				camera_.coordinate_system(),
				camera_.projection_matrix()),
			sge::shader::matrix_flags::projection));

	shader_.update_uniform(
		"world",
		sge::shader::matrix(
			sge::camera::matrix_conversion::world(
				camera_.coordinate_system()),
			sge::shader::matrix_flags::none));

	sge::renderer::scoped_vertex_buffer
		scoped_positions(
			renderer_,
			*positions_buffer_),
		scoped_normals(
			renderer_,
			*normals_buffer_);

	/*
	FCPPT_ASSERT_PRE(
		vertex_count_ % 3 == 0);*/

	renderer_.render_nonindexed(
		sge::renderer::first_vertex(
			0u),
		sge::renderer::vertex_count(
			vertex_count_),
		sge::renderer::nonindexed_primitive_type::triangle);
}

flakelib::marching_cubes::object::~object()
{
}

void
flakelib::marching_cubes::object::launch_classifyVoxel(
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
flakelib::marching_cubes::object::launch_compactVoxels(
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
flakelib::marching_cubes::object::launch_generateTriangles2(
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
	uint activeVoxels)
{
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
		vertex_count_);
    ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 11, sizeof(uint), &cl_vertex_count);
    FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);

    ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 12, sizeof(cl_mem), &num_vert_table_);
    FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
    ciErrNum = clSetKernelArg(generateTriangles2Kernel.impl(), 13, sizeof(cl_mem), &triangle_table_);
    FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);

    grid.w() *= threads.w();
    ciErrNum = clEnqueueNDRangeKernel(command_queue_.impl(), generateTriangles2Kernel.impl(), 1, NULL, (size_t*) grid.data(), (size_t*) threads.data(), 0, 0, 0);
    FCPPT_ASSERT_ERROR(ciErrNum == CL_SUCCESS);
}

void
flakelib::marching_cubes::object::resize_gl_buffers()
{
	if(!vertex_count_)
		return;

	if(positions_buffer_ && vertex_count_ < positions_buffer_->size().get())
		return;

	sge::renderer::vertex_count const real_vertex_count(
		static_cast<sge::renderer::size_type>(
			static_cast<double>(
				vertex_count_) *
			1.5));

	positions_buffer_ =
		renderer_.create_vertex_buffer(
			*vertex_declaration_,
			sge::renderer::vf::dynamic::make_part_index
			<
				vf::format,
				vf::position_part
			>(),
			real_vertex_count,
			sge::renderer::resource_flags::none);

	normals_buffer_ =
		renderer_.create_vertex_buffer(
			*vertex_declaration_,
			sge::renderer::vf::dynamic::make_part_index
			<
				vf::format,
				vf::normal_part
			>(),
			real_vertex_count,
			sge::renderer::resource_flags::none);

	positions_buffer_cl_.take(
		fcppt::make_unique_ptr<sge::opencl::memory_object::buffer>(
			fcppt::ref(
				command_queue_.context()),
			fcppt::ref(
				*positions_buffer_),
			sge::opencl::memory_object::renderer_buffer_lock_mode::read_write));

	normals_buffer_cl_.take(
		fcppt::make_unique_ptr<sge::opencl::memory_object::buffer>(
			fcppt::ref(
				command_queue_.context()),
			fcppt::ref(
				*normals_buffer_),
			sge::opencl::memory_object::renderer_buffer_lock_mode::read_write));
}
