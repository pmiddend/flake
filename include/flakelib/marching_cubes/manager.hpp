#ifndef FLAKELIB_MARCHING_CUBES_MANAGER_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_MANAGER_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/marching_cubes/object.hpp>
#include <flakelib/marching_cubes/scan.hpp>
#include <flakelib/volume/gradient_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/dim3.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/size_type.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace marching_cubes
{
class manager
{
FCPPT_NONCOPYABLE(
	manager);
public:
	FLAKELIB_SYMBOL
	manager(
		sge::renderer::device &,
		flakelib::volume::gradient &,
		flakelib::cl::program_context const &);

	FLAKELIB_SYMBOL
	void
	render();

	FLAKELIB_SYMBOL
	sge::renderer::vertex_declaration &
	vertex_declaration();

	FLAKELIB_SYMBOL
	~manager();
private:
	friend class flakelib::marching_cubes::object;

	typedef
	boost::intrusive::list
	<
		flakelib::marching_cubes::object,
		boost::intrusive::constant_time_size<false>
	>
	child_sequence;

	sge::opencl::command_queue::object &command_queue_;
	sge::renderer::device &renderer_;
	sge::renderer::vertex_declaration_scoped_ptr vertex_declaration_;
	flakelib::marching_cubes::scan scan_;
	child_sequence children_;
	cl_image_format table_format_;
	cl_int triangle_table_error_code_;
	cl_int num_vert_table_error_code_;
	cl_mem triangle_table_;
	cl_mem num_vert_table_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object classifyVoxelKernel;
	sge::opencl::kernel::object compactVoxelsKernel;
	sge::opencl::kernel::object generateTriangles2Kernel;
	flakelib::volume::gradient &gradient_;

	void
	add_child(
		flakelib::marching_cubes::object &);

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
		uint activeVoxels,
		sge::renderer::size_type);
};
}
}

#endif

