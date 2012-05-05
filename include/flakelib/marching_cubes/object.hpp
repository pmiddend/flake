#ifndef FLAKELIB_MARCHING_CUBES_OBJECT_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_OBJECT_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/marching_cubes/iso_level.hpp>
#include <flakelib/marching_cubes/manager_fwd.hpp>
#include <flakelib/marching_cubes/vertex_count.hpp>
#include <flakelib/volume/float_view.hpp>
#include <flakelib/volume/grid_size.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/renderer/vertex_buffer_scoped_ptr.hpp>
#include <sge/shader/object_fwd.hpp>
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
		flakelib::marching_cubes::manager &,
		sge::shader::object &,
		flakelib::volume::grid_size const &,
		flakelib::marching_cubes::iso_level const &);

	FLAKELIB_SYMBOL
	flakelib::marching_cubes::vertex_count const
	update(
		flakelib::volume::float_view const &);

	FLAKELIB_SYMBOL
	~object();
private:
	friend class flakelib::marching_cubes::manager;

	flakelib::marching_cubes::manager &manager_;
	sge::shader::object &shader_;
	sge::opencl::command_queue::object &command_queue_;
	flakelib::volume::grid_size grid_size_;
	flakelib::marching_cubes::iso_level iso_level_;
	sge::renderer::vertex_buffer_scoped_ptr positions_buffer_;
	sge::renderer::vertex_buffer_scoped_ptr normals_buffer_;
	fcppt::scoped_ptr<sge::opencl::memory_object::buffer> positions_buffer_cl_;
	fcppt::scoped_ptr<sge::opencl::memory_object::buffer> normals_buffer_cl_;
	sge::renderer::size_type vertex_count_;
	sge::opencl::memory_object::buffer voxel_verts_;
	sge::opencl::memory_object::buffer voxel_verts_scan_;
	sge::opencl::memory_object::buffer voxel_occupied_;
	sge::opencl::memory_object::buffer voxel_occupied_scan_;
	sge::opencl::memory_object::buffer voxel_array_;

	void
	render();

	void
	resize_gl_buffers();
};

FCPPT_PP_POP_WARNING
}
}

#endif

