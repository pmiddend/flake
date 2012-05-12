#ifndef FLAKE_VOLUME_TESTS_DENSITY_VISUALIZATION_HPP_INCLUDED
#define FLAKE_VOLUME_TESTS_DENSITY_VISUALIZATION_HPP_INCLUDED

#include <flake/test/base.hpp>
#include <flake/volume/density_visualization/raycaster/object.hpp>
#include <flakelib/splatter/object.hpp>
#include <flakelib/utility/fill_buffer.hpp>
#include <flakelib/volume/grid_size.hpp>
#include <flakelib/volume/unique_float_buffer_lock.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <sge/camera/perspective_projection_from_viewport.hpp>
#include <sge/camera/first_person/object.hpp>
#include <sge/timer/basic.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <awl/main/exit_code.hpp>
#include <awl/main/function_context_fwd.hpp>
#include <fcppt/noncopyable.hpp>


namespace flake
{
namespace volume
{
namespace tests
{
class density_visualization
:
	public flake::test::base
{
FCPPT_NONCOPYABLE(
	density_visualization);
public:
	explicit
	density_visualization(
		awl::main::function_context const &);

	awl::main::exit_code const
	run();

	~density_visualization();
private:
	flakelib::volume::grid_size const simulation_size_;

	sge::camera::first_person::object camera_;
	sge::camera::perspective_projection_from_viewport perspective_projection_from_viewport_;

	flakelib::utility::fill_buffer fill_buffer_;
	flakelib::splatter::object splatter_;

	flakelib::volume::unique_float_buffer_lock density_buffer_;

	flakelib::volume::conversion::object conversion_;
	flake::volume::density_visualization::raycaster::object raycaster_;

	// Timer
	sge::timer::basic<sge::timer::clocks::standard> delta_timer_;

	void
	render(
		sge::renderer::context::object &);

	void
	update();
};
}
}
}

#endif

