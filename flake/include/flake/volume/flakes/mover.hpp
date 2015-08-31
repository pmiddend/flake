#ifndef FLAKE_VOLUME_FLAKES_MOVER_HPP_INCLUDED
#define FLAKE_VOLUME_FLAKES_MOVER_HPP_INCLUDED

#include <flake/detail/symbol.hpp>
#include <flake/volume/flakes/activity_view.hpp>
#include <flake/volume/flakes/collision_increment.hpp>
#include <flake/volume/flakes/count.hpp>
#include <flake/volume/flakes/gravity_magnitude.hpp>
#include <flake/volume/flakes/maximum_size.hpp>
#include <flake/volume/flakes/minimum_size.hpp>
#include <flake/volume/flakes/point_size_view.hpp>
#include <flake/volume/flakes/position_view.hpp>
#include <flake/volume/flakes/snow_density_view.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/buffer_pool/linear_lock.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/cl/kernel_unique_ptr.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/marching_cubes/iso_level.hpp>
#include <flakelib/volume/boundary_buffer_view.hpp>
#include <flakelib/volume/float_view.hpp>
#include <flakelib/volume/unique_float4_buffer_lock.hpp>
#include <flakelib/volume/velocity_buffer_view.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/unique_ptr.hpp>


namespace flake
{
namespace volume
{
namespace flakes
{
class mover
{
FCPPT_NONCOPYABLE(
	mover);
public:
	FLAKE_DETAIL_SYMBOL
	mover(
		flakelib::cl::program_context const &,
		flakelib::buffer_pool::object &,
		flake::volume::flakes::position_view const &,
		flake::volume::flakes::point_size_view const &,
		flake::volume::flakes::snow_density_view const &,
		flake::volume::flakes::collision_increment const &,
		flake::volume::flakes::activity_view const &,
		flake::volume::flakes::minimum_size const &,
		flake::volume::flakes::maximum_size const &,
		flake::volume::flakes::gravity_magnitude const &,
		flakelib::marching_cubes::iso_level const &);

	FLAKE_DETAIL_SYMBOL
	void
	update(
		flakelib::duration const &,
		flakelib::volume::velocity_buffer_view const &,
		flakelib::volume::boundary_buffer_view const &,
		flake::volume::flakes::count const &);

	FLAKE_DETAIL_SYMBOL
	~mover();
private:
	typedef
	flakelib::buffer::linear_view<cl_float4>
	linear_float4_view;

	typedef
	flakelib::buffer_pool::linear_lock<cl_float4>
	linear_float4_lock;

	typedef
	fcppt::unique_ptr<linear_float4_lock>
	unique_linear_float4_lock;

	flakelib::cl::program program_;
	flakelib::cl::kernel_unique_ptr const initialize_velocities_kernel_;
	flakelib::cl::kernel_unique_ptr const move_kernel_;
	flakelib::cl::kernel_unique_ptr const update_activity_kernel_;
	flake::volume::flakes::position_view positions_;
	flake::volume::flakes::point_size_view point_sizes_;
	flake::volume::flakes::activity_view activity_;
	unique_linear_float4_lock velocities_;
	cl_float time_;

	void
	initialize_velocities(
		flake::volume::flakes::minimum_size const &,
		flake::volume::flakes::maximum_size const &);

	void
	update_activity(
		flakelib::volume::boundary_buffer_view const &);
};
}
}
}

#endif

