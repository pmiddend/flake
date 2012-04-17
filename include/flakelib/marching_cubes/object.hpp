#ifndef FLAKELIB_MARCHING_CUBES_OBJECT_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_OBJECT_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/marching_cubes/iso_level.hpp>
#include <flakelib/marching_cubes/scan.hpp>
#include <flakelib/marching_cubes/vertex_count.hpp>
#include <flakelib/volume/float_view.hpp>
#include <flakelib/volume/gradient.hpp>
#include <flakelib/volume/grid_size.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_buffer_unique_ptr.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/shader/object.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <fcppt/math/dim/object_impl.hpp>


namespace flakelib
{
namespace marching_cubes
{
class object
{
public:
	FLAKELIB_SYMBOL
	explicit
	object(
		sge::renderer::device &,
		sge::camera::base &,
		flakelib::volume::gradient &,
		flakelib::cl::program_context const &,
		flakelib::volume::grid_size const &,
		marching_cubes::iso_level const &);

	FLAKELIB_SYMBOL
	marching_cubes::vertex_count const
	update(
		flakelib::volume::float_view const &);

	FLAKELIB_SYMBOL
	void
	render();

	FLAKELIB_SYMBOL
	~object();
private:
	sge::renderer::device &renderer_;
	sge::camera::base &camera_;
	flakelib::volume::gradient &gradient_;
	sge::opencl::command_queue::object &command_queue_;
	flakelib::volume::grid_size grid_size_;
	marching_cubes::iso_level iso_level_;
	sge::renderer::vertex_declaration_scoped_ptr vertex_declaration_;
	sge::shader::object shader_;
	sge::renderer::vertex_buffer_unique_ptr positions_buffer_;
	sge::renderer::vertex_buffer_unique_ptr normals_buffer_;
	fcppt::scoped_ptr<sge::opencl::memory_object::buffer> positions_buffer_cl_;
	fcppt::scoped_ptr<sge::opencl::memory_object::buffer> normals_buffer_cl_;
	sge::renderer::size_type vertex_count_;

	marching_cubes::scan scan_;

	cl_image_format table_format_;
	cl_int triangle_table_error_code_;
	cl_int num_vert_table_error_code_;
	cl_mem triangle_table_;
	cl_mem num_vert_table_;

	sge::opencl::memory_object::buffer voxel_verts_;
	sge::opencl::memory_object::buffer voxel_verts_scan_;
	sge::opencl::memory_object::buffer voxel_occupied_;
	sge::opencl::memory_object::buffer voxel_occupied_scan_;
	sge::opencl::memory_object::buffer voxel_array_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object classifyVoxelKernel;
	sge::opencl::kernel::object compactVoxelsKernel;
	sge::opencl::kernel::object generateTriangles2Kernel;

	void
	launch_classifyVoxel(
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
		float isoValue);

	void
	launch_compactVoxels(
		sge::opencl::memory_object::dim3 grid,
		sge::opencl::memory_object::dim3 threads,
		cl_mem compVoxelArray,
		cl_mem voxelOccupied,
		cl_mem voxelOccupiedScan,
		uint numVoxels);

	void
	launch_generateTriangles2(
		flakelib::volume::float_view const &,
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
		uint activeVoxels);

	void
	resize_gl_buffers();

};
}
}

#endif

