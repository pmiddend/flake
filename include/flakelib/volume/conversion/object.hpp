#ifndef FLAKELIB_VOLUME_CONVERSION_OBJECT_HPP_INCLUDED
#define FLAKELIB_VOLUME_CONVERSION_OBJECT_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/cl/unique_kernel_ptr.hpp>
#include <flakelib/volume/float4_view.hpp>
#include <flakelib/volume/float_view.hpp>
#include <flakelib/volume/conversion/arrow_scale.hpp>
#include <flakelib/volume/conversion/constant_addition.hpp>
#include <flakelib/volume/conversion/grid_scale.hpp>
#include <flakelib/volume/conversion/origin.hpp>
#include <flakelib/volume/conversion/scaling_factor.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace volume
{
namespace conversion
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	FLAKELIB_SYMBOL
	explicit
	object(
		cl::program_context const &);

	FLAKELIB_SYMBOL
	void
	to_arrow_vb(
		volume::float4_view const &,
		sge::opencl::memory_object::buffer &,
		conversion::arrow_scale const &,
		conversion::origin const &,
		conversion::grid_scale const &);

	FLAKELIB_SYMBOL
	void
	float_view_to_flat_volume_texture(
		volume::float_view const &,
		sge::opencl::memory_object::image::planar &,
		conversion::scaling_factor const &,
		conversion::constant_addition const &);

	FLAKELIB_SYMBOL
	~object();
private:
	cl::program program_;
	cl::unique_kernel_ptr to_arrow_vb_kernel_;
	cl::unique_kernel_ptr float_view_to_flat_volume_texture_kernel_;
};
}
}
}

#endif

