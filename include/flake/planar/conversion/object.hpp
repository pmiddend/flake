#ifndef FLAKE_PLANAR_CONVERSION_OBJECT_HPP_INCLUDED
#define FLAKE_PLANAR_CONVERSION_OBJECT_HPP_INCLUDED

#include <flake/planar/conversion/arrow_scale.hpp>
#include <flake/planar/conversion/constant_addition.hpp>
#include <flake/planar/conversion/grid_scale.hpp>
#include <flake/planar/conversion/scaling_factor.hpp>
#include <flakelib/buffer/planar_view_fwd.hpp>
#include <flakelib/cl/kernel_scoped_ptr.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/planar/float2_view.hpp>
#include <flakelib/planar/float_view.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>

namespace flake
{
namespace planar
{
namespace conversion
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		flakelib::cl::program_context const &);

	void
	to_arrow_vb(
		flakelib::planar::float2_view const &,
		// Technically a planar buffer as well, but one coming from
		// OpenGL.
		sge::opencl::memory_object::buffer &,
		conversion::grid_scale const &,
		conversion::arrow_scale const &);

	void
	scalar_to_texture(
		flakelib::planar::float_view const &,
		sge::opencl::memory_object::image::planar &,
		conversion::scaling_factor const &,
		conversion::constant_addition const &);

	~object();
private:
	sge::opencl::command_queue::object &command_queue_;
	flakelib::cl::program program_;
	flakelib::cl::kernel_scoped_ptr to_arrow_vb_kernel_;
	flakelib::cl::kernel_scoped_ptr scalar_to_texture_kernel_;
};
}
}
}

#endif

