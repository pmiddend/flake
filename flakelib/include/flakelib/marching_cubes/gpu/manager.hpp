#ifndef FLAKELIB_MARCHING_CUBES_MANAGER_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_MANAGER_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/buffer/linear_view.hpp>
#include <flakelib/buffer_pool/linear_lock.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/kernel_scoped_ptr.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/marching_cubes/iso_level.hpp>
#include <flakelib/marching_cubes/gpu/active_voxels.hpp>
#include <flakelib/marching_cubes/gpu/compacted_voxel_occupation_view.hpp>
#include <flakelib/marching_cubes/gpu/density_view.hpp>
#include <flakelib/marching_cubes/gpu/grid_size.hpp>
#include <flakelib/marching_cubes/gpu/grid_size_mask.hpp>
#include <flakelib/marching_cubes/gpu/grid_size_shift.hpp>
#include <flakelib/marching_cubes/gpu/normals_buffer.hpp>
#include <flakelib/marching_cubes/gpu/object.hpp>
#include <flakelib/marching_cubes/gpu/positions_buffer.hpp>
#include <flakelib/marching_cubes/gpu/summed_vertices_for_voxel_view.hpp>
#include <flakelib/marching_cubes/gpu/summed_voxel_occupation_view.hpp>
#include <flakelib/marching_cubes/gpu/vertices_for_voxel_view.hpp>
#include <flakelib/marching_cubes/gpu/voxel_occupation_view.hpp>
#include <flakelib/scan/object_fwd.hpp>
#include <flakelib/volume/gradient_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/dim3.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/renderer/size_type.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/context/core_fwd.hpp>
#include <sge/renderer/device/core_fwd.hpp>
#include <sge/renderer/state/core/depth_stencil/object_scoped_ptr.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/unique_ptr.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace marching_cubes
{
namespace gpu
{
class manager
{
FCPPT_NONCOPYABLE(
	manager);
public:
	FLAKELIB_SYMBOL
	manager(
		sge::renderer::device::core &,
		flakelib::scan::object &,
		flakelib::volume::gradient &,
		flakelib::cl::program_context const &,
		flakelib::buffer_pool::object &);

	FLAKELIB_SYMBOL
	void
	render(
		sge::renderer::context::core &);

	FLAKELIB_SYMBOL
	sge::renderer::vertex_declaration &
	vertex_declaration();

	FLAKELIB_SYMBOL
	void
	classify_voxels(
		flakelib::marching_cubes::gpu::density_view const &,
		flakelib::marching_cubes::gpu::vertices_for_voxel_view const &,
		flakelib::marching_cubes::gpu::voxel_occupation_view const &,
		flakelib::marching_cubes::iso_level const &,
		flakelib::marching_cubes::gpu::grid_size const &,
		flakelib::marching_cubes::gpu::grid_size_mask const &,
		flakelib::marching_cubes::gpu::grid_size_shift const &);

	FLAKELIB_SYMBOL
	void
	compact_voxels(
		flakelib::marching_cubes::gpu::voxel_occupation_view const &,
		flakelib::marching_cubes::gpu::summed_voxel_occupation_view const &,
		flakelib::marching_cubes::gpu::compacted_voxel_occupation_view const &);

	FLAKELIB_SYMBOL
	void
	generate_triangles(
		flakelib::marching_cubes::gpu::normals_buffer const &,
		flakelib::marching_cubes::gpu::positions_buffer const &,
		flakelib::marching_cubes::gpu::compacted_voxel_occupation_view const &,
		flakelib::marching_cubes::gpu::summed_vertices_for_voxel_view const &,
		flakelib::marching_cubes::gpu::density_view const &,
		flakelib::marching_cubes::gpu::grid_size const &,
		flakelib::marching_cubes::gpu::grid_size_mask const &,
		flakelib::marching_cubes::gpu::grid_size_shift const &,
		flakelib::marching_cubes::iso_level const &,
		flakelib::marching_cubes::gpu::active_voxels const &,
		flakelib::marching_cubes::gpu::vertex_count const &);

	FLAKELIB_SYMBOL
	flakelib::scan::object &
	scan() const;

	FLAKELIB_SYMBOL
	sge::renderer::device::core &
	renderer() const;

	FLAKELIB_SYMBOL
	sge::renderer::vertex_declaration const &
	vertex_declaration() const;

	FLAKELIB_SYMBOL
	~manager();
private:
	friend class flakelib::marching_cubes::gpu::object;

	typedef
	boost::intrusive::list
	<
		flakelib::marching_cubes::gpu::object,
		boost::intrusive::constant_time_size<false>
	>
	child_sequence;

	typedef
	flakelib::buffer::linear_view<cl_uint>
	linear_uint_view;

	typedef
	flakelib::buffer_pool::linear_lock<cl_uint>
	linear_uint_lock;

	typedef
	fcppt::unique_ptr<linear_uint_lock>
	unique_linear_uint_lock;

	sge::opencl::command_queue::object &command_queue_;
	sge::renderer::device::core &renderer_;
	flakelib::scan::object &scan_;
	sge::renderer::vertex_declaration_scoped_ptr const vertex_declaration_;
	sge::renderer::state::core::depth_stencil::object_scoped_ptr const depth_stencil_state_;
	child_sequence children_;
	cl_image_format table_format_;
	cl_int triangle_table_error_code_;
	cl_int num_vert_table_error_code_;
	cl_mem triangle_table_;
	cl_mem num_vert_table_;

	flakelib::cl::program program_;
	cl::kernel_scoped_ptr classify_kernel_;
	cl::kernel_scoped_ptr compact_kernel_;
	cl::kernel_scoped_ptr generate_triangles_kernel_;
	flakelib::volume::gradient &gradient_;
	unique_linear_uint_lock debug_buffer_;

	void
	add_child(
		flakelib::marching_cubes::gpu::object &);

	void
	launch_classifyVoxel(
		sge::opencl::dim3 grid,
		sge::opencl::dim3 threads,
		cl_mem voxelVerts,
		cl_mem voxelOccupied,
		cl_mem volume,
		cl_uint gridSize[4],
		cl_uint gridSizeShift[4],
		cl_uint gridSizeMask[4],
		cl_uint numVoxels,
		cl_float voxelSize[4],
		float isoValue);

	void
	launch_compactVoxels(
		sge::opencl::dim3 grid,
		sge::opencl::dim3 threads,
		cl_mem compVoxelArray,
		cl_mem voxelOccupied,
		cl_mem voxelOccupiedScan,
		cl_uint numVoxels);

	void
	launch_generateTriangles2(
		flakelib::volume::float_view const &,
		sge::opencl::dim3 grid,
		sge::opencl::dim3 threads,
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
		cl_uint activeVoxels,
		sge::renderer::size_type);

#ifdef FLAKELIB_MARCHING_CUBES_GPU_DEBUG
	void
	check_debug_buffer(
		std::string const &_operation);
#endif
};
}
}
}

#endif

