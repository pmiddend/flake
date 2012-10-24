#ifndef FLAKELIB_MARCHING_CUBES_CPU_OBJECT_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_CPU_OBJECT_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/marching_cubes/iso_level.hpp>
#include <flakelib/marching_cubes/cpu/grid_size.hpp>
#include <flakelib/marching_cubes/cpu/implementation_fwd.hpp>
#include <flakelib/marching_cubes/cpu/scalar.hpp>
#include <flakelib/volume/boundary_buffer_view.hpp>
#include <flakelib/volume/float_view.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/renderer/index_buffer_scoped_ptr.hpp>
#include <sge/renderer/index_count.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vertex_buffer_scoped_ptr.hpp>
#include <sge/renderer/vertex_count.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/context/core_fwd.hpp>
#include <sge/renderer/device/core_fwd.hpp>
#include <sge/renderer/index/i32.hpp>
#include <sge/renderer/state/core/depth_stencil/object_scoped_ptr.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <fcppt/container/raw_vector.hpp>


namespace flakelib
{
namespace marching_cubes
{
namespace cpu
{
class object
{
	FCPPT_NONCOPYABLE(
		object);
public:
	FLAKELIB_SYMBOL
	object(
		sge::renderer::device::core &,
		sge::opencl::command_queue::object &,
		flakelib::marching_cubes::cpu::grid_size const &,
		flakelib::marching_cubes::iso_level const &,
		flakelib::volume::boundary_buffer_view const &);

	FLAKELIB_SYMBOL
	void
	render(
		sge::renderer::context::core &);

	FLAKELIB_SYMBOL
	void
	construct_from_raw_data(
		sge::renderer::scalar const *);

	FLAKELIB_SYMBOL
	void
	construct_from_cl_buffer(
		sge::opencl::command_queue::object &,
		flakelib::volume::float_view const &);

	FLAKELIB_SYMBOL
	void
	run();

	FLAKELIB_SYMBOL
	void
	update_buffers();

	FLAKELIB_SYMBOL
	sge::renderer::vertex_declaration &
	vertex_declaration();

	FLAKELIB_SYMBOL
	bool
	is_dirty() const;

	FLAKELIB_SYMBOL
	~object();
private:
	typedef
	fcppt::container::raw_vector<flakelib::marching_cubes::cpu::scalar>
	real_sequence;

	typedef
	fcppt::container::raw_vector<sge::renderer::scalar>
	renderer_scalar_sequence;

	typedef
	fcppt::container::raw_vector<cl_float>
	cl_float_sequence;

	typedef
	fcppt::container::raw_vector<sge::renderer::index::i32>
	index_sequence;

	sge::renderer::device::core &renderer_;
	flakelib::marching_cubes::cpu::grid_size const grid_size_;
	flakelib::marching_cubes::iso_level const iso_level_;
	sge::renderer::vertex_declaration_scoped_ptr vertex_declaration_;
	sge::renderer::vertex_buffer_scoped_ptr vertex_buffer_;
	sge::renderer::vertex_count vertex_buffer_size_;
	sge::renderer::index_buffer_scoped_ptr index_buffer_;
	sge::renderer::index_count index_buffer_size_;
	sge::renderer::state::core::depth_stencil::object_scoped_ptr const depth_stencil_state_;
	fcppt::scoped_ptr<MarchingCubes> const implementation_;
	real_sequence data_;
	renderer_scalar_sequence vertex_buffer_data_;
	index_sequence index_data_;
	bool is_dirty_;
	cl_float_sequence boundary_;

	void
	fill_vertex_buffer();

	void
	fill_index_buffer();
};
}
}
}

#endif