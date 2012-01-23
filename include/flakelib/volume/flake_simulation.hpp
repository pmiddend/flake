#ifndef FLAKELIB_VOLUME_FLAKE_SIMULATION_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLAKE_SIMULATION_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <flakelib/volume/boundary/object_fwd.hpp>
#include <flakelib/volume/conversion/object_fwd.hpp>
#include <flakelib/volume/flakes/object_fwd.hpp>
#include <flakelib/volume/laplace_solver/unique_base_ptr.hpp>
#include <flakelib/volume/simulation/stam/object_fwd.hpp>
#include <flakelib/volume/simulation/stam/wind_blower_fwd.hpp>
#include <flakelib/volume/visualization/arrows_fwd.hpp>
#include <flakelib/volume/visualization/arrows_manager_fwd.hpp>
#include <flakelib/volume/visualization/ground_fwd.hpp>
#include <flakelib/volume/visualization/shape_manager_fwd.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/memory_object/dim3.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/scalar.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/scoped_ptr.hpp>


namespace flakelib
{
namespace volume
{
class flake_simulation
{
FCPPT_NONCOPYABLE(
	flake_simulation);
public:
	explicit
	flake_simulation(
		sge::opencl::command_queue::object &,
		sge::renderer::device &,
		sge::camera::base &,
		sge::image2d::system &,
		flakelib::build_options const &,
		sge::parse::json::object const &);

	void
	update(
		flakelib::duration const &);

	void
	render();

	void
	toggle_arrows();

	void
	external_force_magnitude(
		cl_float);

	void
	flake_size_multiplier(
		sge::renderer::scalar);

	~flake_simulation();
private:
	sge::camera::base &camera_;
	fcppt::scoped_ptr<flakelib::utility::object> utility_;
	fcppt::scoped_ptr<buffer_pool::object> buffer_pool_;
	fcppt::scoped_ptr<boundary::object> boundary_;
	laplace_solver::unique_base_ptr laplace_solver_;
	fcppt::scoped_ptr<simulation::stam::object> simulation_;
	fcppt::scoped_ptr<visualization::shape_manager> shape_manager_;
	fcppt::scoped_ptr<conversion::object> conversion_;
	fcppt::scoped_ptr<visualization::arrows_manager> arrows_manager_;
	fcppt::scoped_ptr<visualization::arrows> arrows_;
	fcppt::scoped_ptr<flakes::object> flakes_;
	fcppt::scoped_ptr<visualization::ground> ground_;
	fcppt::scoped_ptr<simulation::stam::wind_blower> wind_blower_;
	bool draw_arrows_;
};
}
}

#endif
