#ifndef FLAKE_VOLUME_FLAKES_MOVER_HPP_INCLUDED
#define FLAKE_VOLUME_FLAKES_MOVER_HPP_INCLUDED

#include <flake/volume/flakes/position_view.hpp>
#include <flake/volume/flakes/snow_density_view.hpp>
#include <flake/volume/flakes/collision_increment.hpp>
#include <flakelib/marching_cubes/iso_level.hpp>
#include <flake/volume/flakes/activity_view.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/unique_kernel_ptr.hpp>
#include <flakelib/volume/velocity_buffer_view.hpp>
#include <flakelib/volume/float_view.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/volume/boundary_buffer_view.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/memory_object/size_type.hpp>
#include <fcppt/noncopyable.hpp>

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
	mover(
		flakelib::cl::program_context const &,
		flakes::position_view const &,
		flakes::snow_density_view const &,
		flakes::collision_increment const &,
		flakes::activity_view const &,
		flakelib::marching_cubes::iso_level const &);

	void
	update(
		flakelib::duration const &,
		flakelib::volume::velocity_buffer_view const &,
		flakelib::volume::boundary_buffer_view const &);

	~mover();
private:
	flakelib::cl::program program_;
	flakelib::cl::unique_kernel_ptr move_kernel_;
	flakelib::cl::unique_kernel_ptr update_activity_kernel_;
	flakes::position_view positions_;
	flakes::activity_view activity_;
	sge::opencl::memory_object::size_type const vertex_count_;
};
}
}
}

#endif

