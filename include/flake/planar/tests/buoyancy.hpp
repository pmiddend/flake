#ifndef FLAKE_PLANAR_TESTS_BUOYANCY_HPP_INCLUDED
#define FLAKE_PLANAR_TESTS_BUOYANCY_HPP_INCLUDED

#include <flake/planar/cursor_splatter.hpp>
#include <flake/planar/monitor/parent.hpp>
#include <flake/planar/monitor/planar_arrows.hpp>
#include <flake/planar/monitor/planar_converter.hpp>
#include <flake/planar/monitor/texture.hpp>
#include <flake/test/base.hpp>
#include <flakelib/buffer_pool/planar_lock_impl.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/planar/float_buffer_lock.hpp>
#include <flakelib/planar/unique_float2_buffer_lock.hpp>
#include <flakelib/planar/simulation/stam/divergence.hpp>
#include <flakelib/planar/simulation/stam/jacobi.hpp>
#include <flakelib/planar/simulation/stam/outflow_boundaries.hpp>
#include <flakelib/planar/simulation/stam/semilagrangian_advection.hpp>
#include <flakelib/planar/simulation/stam/subtract_pressure_gradient.hpp>
#include <flakelib/planar/simulation/stam/vorticity.hpp>
#include <flakelib/planar/simulation/stam/wind_source.hpp>
#include <flakelib/planar/simulation/stam/buissnesq/ambient_temperature.hpp>
#include <flakelib/planar/simulation/stam/buissnesq/object.hpp>
#include <flakelib/splatter/object.hpp>
#include <flakelib/utility/fill_buffer.hpp>
#include <flakelib/utility/mix_buffers.hpp>
#include <rucksack/widget/enumeration.hpp>
#include <rucksack/widget/viewport_adaptor.hpp>
#include <sge/camera/ortho_freelook/object.hpp>
#include <sge/camera/ortho_freelook/projection_rectangle_from_viewport.hpp>
#include <sge/image2d/file_unique_ptr.hpp>
#include <sge/renderer/texture/planar_scoped_ptr.hpp>
#include <sge/timer/basic.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <awl/main/exit_code.hpp>
#include <awl/main/function_context_fwd.hpp>
#include <fcppt/noncopyable.hpp>


namespace flake
{
namespace planar
{
namespace tests
{
class buoyancy
:
	public flake::test::base
{
FCPPT_NONCOPYABLE(
	buoyancy);
public:
	explicit
	buoyancy(
		awl::main::function_context const &);

	awl::main::exit_code const
	run();

	~buoyancy();
private:
	flakelib::planar::simulation::stam::buissnesq::ambient_temperature buissnesq_ambient_temperature_;

	// Programs
	flakelib::utility::fill_buffer fill_buffer_;
	flakelib::utility::mix_buffers mix_buffers_;
	flakelib::splatter::object splatter_;
	flakelib::planar::simulation::stam::outflow_boundaries outflow_boundaries_;
	flakelib::planar::simulation::stam::semilagrangian_advection semilagrangian_advection_;
	flakelib::planar::simulation::stam::divergence divergence_;
	flakelib::planar::simulation::stam::jacobi jacobi_;
	flakelib::planar::simulation::stam::subtract_pressure_gradient subtract_pressure_gradient_;
	flakelib::planar::simulation::stam::vorticity vorticity_;
	flakelib::planar::simulation::stam::buissnesq::object buissnesq_buoyancy_;
	// Buffers
	sge::image2d::file_unique_ptr boundary_image_file_;
	flakelib::planar::float_buffer_lock boundary_buffer_;
	flakelib::planar::unique_float2_buffer_lock velocity_buffer_;
	flakelib::planar::unique_float_buffer_lock smoke_density_buffer_;
	flakelib::planar::unique_float_buffer_lock smoke_density_source_buffer_;
	flakelib::planar::unique_float_buffer_lock temperature_density_buffer_;
	flakelib::planar::unique_float_buffer_lock temperature_density_source_buffer_;

	// Monitors
	sge::renderer::texture::planar_scoped_ptr boundary_texture_;
	flake::planar::monitor::parent monitor_parent_;
	flake::planar::monitor::planar_converter monitor_planar_converter_;
	flake::planar::monitor::planar_arrows velocity_arrows_;
	flake::planar::monitor::texture smoke_density_texture_;
	flake::planar::monitor::texture temperature_density_texture_;
	rucksack::widget::viewport_adaptor rucksack_viewport_adaptor_;
	rucksack::widget::enumeration rucksack_enumeration_;

	// Other stuff
	sge::camera::ortho_freelook::object freelook_camera_;
	sge::camera::ortho_freelook::projection_rectangle_from_viewport projection_rectangle_from_viewport_;
	flake::planar::cursor_splatter density_splatter_;
	flake::planar::cursor_splatter temperature_splatter_;

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

