#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/marching_cubes/manager.hpp>
#include <flakelib/marching_cubes/object.hpp>
#include <flakelib/marching_cubes/vf/format.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/dim3.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/opencl/memory_object/size_type.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vf/dynamic/make_part_index.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/scoped.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/log2.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <cmath>
#include <fcppt/config/external_end.hpp>


flakelib::marching_cubes::object::object(
	flakelib::marching_cubes::manager &_manager,
	sge::shader::object &_shader,
	flakelib::volume::grid_size const &_grid_size,
	marching_cubes::iso_level const &_iso_level)
:
	manager_(
		_manager),
	shader_(
		_shader),
	command_queue_(
		_manager.command_queue_),
	grid_size_(
		_grid_size),
	iso_level_(
		_iso_level),
	positions_buffer_(),
	normals_buffer_(),
	positions_buffer_cl_(),
	normals_buffer_cl_(),
	vertex_count_(
		0u),
	voxel_verts_(
		_manager.command_queue_.context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) |
		sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::byte_size(
			grid_size_.get().content() * sizeof(cl_uint))),
	voxel_verts_scan_(
		_manager.command_queue_.context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) |
		sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::byte_size(
			grid_size_.get().content() * sizeof(cl_uint))),
	voxel_occupied_(
		_manager.command_queue_.context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) |
		sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::byte_size(
			grid_size_.get().content() * sizeof(cl_uint))),
	voxel_occupied_scan_(
		_manager.command_queue_.context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) |
		sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::byte_size(
			grid_size_.get().content() * sizeof(cl_uint))),
	voxel_array_(
		_manager.command_queue_.context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) |
		sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::byte_size(
			grid_size_.get().content() * sizeof(cl_uint)))
{
	manager_.add_child(
		*this);
}

flakelib::marching_cubes::vertex_count const
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

	//std::cout << "Grid size log2: " << gridSizeLog2[0] << ", " << gridSizeLog2[1] << ", " << gridSizeLog2[2] << ", " << gridSizeLog2[3] << "\n";

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

	//std::cout << "Grid size log2: " << gridSize[0] << ", " << gridSize[1] << ", " << gridSize[2] << ", " << gridSize[3] << "\n";

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
	manager_.launch_classifyVoxel(
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
		iso_level_.get());

	//std::size_t const first_scan_result =
		manager_.scan_.scanExclusiveLarge(
			voxel_occupied_scan_.impl(),
			voxel_occupied_.impl(),
			1,
			numVoxels);

	//std::cout << "First scan resulted in " << first_scan_result << "\n";

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

	//	std::cout << "Last element: " << lastElement << ", lastScanElement: " << lastScanElement << "\n";
	}

    if (activeVoxels==0)
        return
		flakelib::marching_cubes::vertex_count(
			0u);

    // compact voxel index array
    manager_.launch_compactVoxels(
	    grid,
	    sge::opencl::memory_object::dim3(
		threads,
		1u,
		1u),
	    voxel_array_.impl(),
	    voxel_occupied_.impl(),
	    voxel_occupied_scan_.impl(),
	    numVoxels);

	//std::size_t const second_scan_result =
		manager_.scan_.scanExclusiveLarge(
			voxel_verts_scan_.impl(),
			voxel_verts_.impl(),
			1,
			numVoxels);

	//std::cout << "Second scan resulted in " << second_scan_result << "\n";


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
			std::ceil(static_cast<cl_float>(activeVoxels) / static_cast<cl_float>(nthreads))),
		1u,
		1u);

	while(grid2.w() > 65535)
	{
		grid2.w()/=2;
		grid2.h()*=2;
	}

	manager_.launch_generateTriangles2(
		_view,
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
		iso_level_.get(),
		activeVoxels,
		vertex_count_);

	return
		flakelib::marching_cubes::vertex_count(
			vertex_count_);
}

flakelib::marching_cubes::object::~object()
{
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

	sge::shader::scoped scoped_shader(
		shader_,
		sge::shader::activate_everything());

	sge::renderer::scoped_vertex_buffer
		scoped_positions(
			manager_.renderer_,
			*positions_buffer_),
		scoped_normals(
			manager_.renderer_,
			*normals_buffer_);

	FCPPT_ASSERT_PRE(
		vertex_count_ % 3 == 0);

	manager_.renderer_.render_nonindexed(
		sge::renderer::first_vertex(
			0u),
		sge::renderer::vertex_count(
			vertex_count_),
		sge::renderer::nonindexed_primitive_type::triangle);
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

	positions_buffer_.take(
		manager_.renderer_.create_vertex_buffer(
			*(manager_.vertex_declaration_),
			sge::renderer::vf::dynamic::make_part_index
			<
				flakelib::marching_cubes::vf::format,
				flakelib::marching_cubes::vf::position_part
			>(),
			real_vertex_count,
			sge::renderer::resource_flags::none));

	normals_buffer_.take(
		manager_.renderer_.create_vertex_buffer(
			*(manager_.vertex_declaration_),
			sge::renderer::vf::dynamic::make_part_index
			<
				flakelib::marching_cubes::vf::format,
				flakelib::marching_cubes::vf::normal_part
			>(),
			real_vertex_count,
			sge::renderer::resource_flags::none));

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
