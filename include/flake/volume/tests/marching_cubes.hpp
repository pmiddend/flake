#ifndef FLAKE_VOLUME_TESTS_MARCHING_CUBES_HPP_INCLUDED
#define FLAKE_VOLUME_TESTS_MARCHING_CUBES_HPP_INCLUDED

#include <flake/test/base.hpp>
#include <flakelib/marching_cubes/object.hpp>
#include <flakelib/splatter/object.hpp>
#include <flakelib/utility/fill_buffer.hpp>
#include <flakelib/volume/grid_size.hpp>
#include <flakelib/volume/unique_float_buffer_lock.hpp>
#include <sge/camera/perspective_projection_from_viewport.hpp>
#include <sge/camera/first_person/object.hpp>
#include <sge/input/keyboard/key_event_fwd.hpp>
#include <sge/opencl/memory_object/dim3.hpp>
#include <sge/timer/basic.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <awl/main/exit_code.hpp>
#include <awl/main/function_context_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/signal/scoped_connection.hpp>

namespace flake
{
namespace volume
{
namespace tests
{
class marching_cubes
:
	public flake::test::base
{
FCPPT_NONCOPYABLE(
	marching_cubes);
public:
	explicit
	marching_cubes(
		awl::main::function_context const &);

	awl::main::exit_code const
	run();

	~marching_cubes();
private:
	flakelib::volume::grid_size const simulation_size_;

	sge::camera::first_person::object camera_;
	sge::camera::perspective_projection_from_viewport perspective_projection_from_viewport_;

	flakelib::utility::fill_buffer fill_buffer_;
	flakelib::splatter::object splatter_;

	flakelib::volume::unique_float_buffer_lock boundary_buffer_;

	flakelib::marching_cubes::object marching_cubes_;

	// Timer
	sge::timer::basic<sge::timer::clocks::standard> delta_timer_;

	void
	render();

	void
	update();
};
}
}
}

#endif

