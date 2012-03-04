#ifndef FLAKE_PLANAR_MONITOR_PLANAR_CONVERTER_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_PLANAR_CONVERTER_HPP_INCLUDED

#include <flakelib/buffer/planar_view_fwd.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/planar/float2_view.hpp>
#include <flakelib/planar/float_view.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/unique_kernel_ptr.hpp>
#include <flake/planar/monitor/arrow_scale.hpp>
#include <flake/planar/monitor/grid_scale.hpp>
#include <flake/planar/monitor/scaling_factor.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>


namespace flake
{
namespace planar
{
namespace monitor
{
class planar_converter
{
FCPPT_NONCOPYABLE(
	planar_converter);
public:
	explicit
	planar_converter(
		flakelib::cl::program_context const &);

	void
	to_arrow_vb(
		flakelib::planar::float2_view const &,
		// Technically a planar buffer as well, but one coming from
		// OpenGL.
		sge::opencl::memory_object::buffer &,
		monitor::grid_scale const &,
		monitor::arrow_scale const &);

	void
	scalar_to_texture(
		flakelib::planar::float_view const &,
		sge::opencl::memory_object::image::planar &,
		monitor::scaling_factor const &);

	~planar_converter();
private:
	sge::opencl::command_queue::object &command_queue_;
	flakelib::cl::program program_;
	flakelib::cl::unique_kernel_ptr to_arrow_vb_kernel_;
	flakelib::cl::unique_kernel_ptr scalar_to_texture_kernel_;
};
}
}
}

#endif
