#ifndef FLAKE_PLANAR_CONVERSION_OBJECT_HPP_INCLUDED
#define FLAKE_PLANAR_CONVERSION_OBJECT_HPP_INCLUDED

#include <flake/detail/symbol.hpp>
#include <flake/planar/conversion/arrow_scale.hpp>
#include <flake/planar/conversion/constant_addition.hpp>
#include <flake/planar/conversion/grid_scale.hpp>
#include <flake/planar/conversion/scaling_factor.hpp>
#include <flake/planar/conversion/x_coordinate_file.hpp>
#include <flake/planar/conversion/y_coordinate_file.hpp>
#include <flakelib/buffer/planar_view_fwd.hpp>
#include <flakelib/cl/kernel_unique_ptr.hpp>
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
	FLAKE_DETAIL_SYMBOL
	explicit
	object(
		flakelib::cl::program_context const &);

	FLAKE_DETAIL_SYMBOL
	void
	to_arrow_vb(
		flakelib::planar::float2_view const &,
		// Technically a planar buffer as well, but one coming from
		// OpenGL.
		sge::opencl::memory_object::buffer &,
		conversion::grid_scale const &,
		conversion::arrow_scale const &);

	FLAKE_DETAIL_SYMBOL
	void
	arrows_to_matlab_files(
		flakelib::planar::float2_view const &,
		flake::planar::conversion::x_coordinate_file const &,
		flake::planar::conversion::y_coordinate_file const &);

	FLAKE_DETAIL_SYMBOL
	void
	scalar_to_matlab_file(
		flakelib::planar::float_view const &,
		boost::filesystem::path const &);

	FLAKE_DETAIL_SYMBOL
	void
	scalar_to_texture(
		flakelib::planar::float_view const &,
		sge::opencl::memory_object::image::planar &,
		conversion::scaling_factor const &,
		conversion::constant_addition const &);

	FLAKE_DETAIL_SYMBOL
	~object();
private:
	sge::opencl::command_queue::object &command_queue_;
	flakelib::cl::program program_;
	flakelib::cl::kernel_unique_ptr const to_arrow_vb_kernel_;
	flakelib::cl::kernel_unique_ptr const scalar_to_texture_kernel_;
};
}
}
}

#endif

