#ifndef FLAKELIB_MARCHING_CUBES_GPU_OBJECT_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_GPU_OBJECT_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/cl/uint4.hpp>
#include <flakelib/linear/uint_view.hpp>
#include <flakelib/marching_cubes/iso_level.hpp>
#include <flakelib/marching_cubes/gpu/density_view.hpp>
#include <flakelib/marching_cubes/gpu/grid_size.hpp>
#include <flakelib/marching_cubes/gpu/grid_size_mask.hpp>
#include <flakelib/marching_cubes/gpu/grid_size_shift.hpp>
#include <flakelib/marching_cubes/gpu/manager_fwd.hpp>
#include <flakelib/marching_cubes/gpu/vertex_count.hpp>
#include <flakelib/volume/float_view.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/renderer/context/core_fwd.hpp>
#include <sge/renderer/vertex/buffer_scoped_ptr.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/preprocessor/disable_gcc_warning.hpp>
#include <fcppt/preprocessor/pop_warning.hpp>
#include <fcppt/preprocessor/push_warning.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace marching_cubes
{
namespace gpu
{
FCPPT_PP_PUSH_WARNING
FCPPT_PP_DISABLE_GCC_WARNING(-Weffc++)

class object
:
	public boost::intrusive::list_base_hook
	<
		boost::intrusive::link_mode<boost::intrusive::auto_unlink>
	>
{
FCPPT_NONCOPYABLE(
	object);
public:
	FLAKELIB_SYMBOL
	object(
		flakelib::buffer_pool::object &,
		flakelib::marching_cubes::gpu::manager &,
		flakelib::marching_cubes::gpu::grid_size const &,
		flakelib::marching_cubes::iso_level const &);

	FLAKELIB_SYMBOL
	flakelib::marching_cubes::gpu::vertex_count const
	update(
		flakelib::marching_cubes::gpu::density_view const &);

	FLAKELIB_SYMBOL
	~object();
private:
	friend class flakelib::marching_cubes::gpu::manager;

	flakelib::marching_cubes::gpu::manager &manager_;
	flakelib::buffer_pool::object &buffer_pool_;
	sge::opencl::command_queue::object &command_queue_;
	flakelib::marching_cubes::gpu::grid_size const grid_size_;
	flakelib::cl::uint4 const grid_size_log2_;
	flakelib::marching_cubes::gpu::grid_size_mask const grid_size_mask_;
	flakelib::marching_cubes::gpu::grid_size_shift const grid_size_shift_;
	flakelib::marching_cubes::iso_level iso_level_;
	sge::renderer::vertex::buffer_scoped_ptr positions_buffer_;
	sge::renderer::vertex::buffer_scoped_ptr normals_buffer_;
	fcppt::scoped_ptr<sge::opencl::memory_object::buffer> positions_buffer_cl_;
	fcppt::scoped_ptr<sge::opencl::memory_object::buffer> normals_buffer_cl_;
	flakelib::marching_cubes::gpu::vertex_count vertex_count_;

	void
	render(
		sge::renderer::context::core &);

	void
	resize_gl_buffers();

	cl_uint
	sum_last_elements(
		flakelib::linear::uint_view const &,
		flakelib::linear::uint_view const &);
};

FCPPT_PP_POP_WARNING
}
}
}

#endif

