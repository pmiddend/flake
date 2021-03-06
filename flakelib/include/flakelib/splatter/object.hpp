#ifndef FLAKELIB_SPLATTER_OBJECT_HPP_INCLUDED
#define FLAKELIB_SPLATTER_OBJECT_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/cl/float2.hpp>
#include <flakelib/cl/kernel_unique_ptr.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/planar/float2_view.hpp>
#include <flakelib/planar/float_view.hpp>
#include <flakelib/splatter/pen/planar.hpp>
#include <flakelib/splatter/pen/volume.hpp>
#include <flakelib/volume/float_view.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace splatter
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
	splat_planar_float(
		planar::float_view const &,
		splatter::pen::planar const &,
		cl_float);

	FLAKELIB_SYMBOL
	void
	splat_planar_float2(
		planar::float_view const &,
		splatter::pen::planar const &,
		cl::float2 const &);

	FLAKELIB_SYMBOL
	void
	splat_volume_float(
		volume::float_view const &,
		splatter::pen::volume const &,
		cl_float);

	/*
	void
	splat_float4_sphere(
		volume:float4_view const &,
		splat::sphere const &,
		cl_float4);

	void
	splat_float_box(
		volume::float_view const &,
		splat::box const &,
		cl_float);

	void
	splat_float4_box(
		volume::float4_view const &,
		splat::box const &,
		cl_float4);
	*/

	FLAKELIB_SYMBOL
	~object();
private:
	flakelib::cl::program program_;
	flakelib::cl::kernel_unique_ptr const splat_planar_float_;
	flakelib::cl::kernel_unique_ptr const splat_planar_float2_;
	flakelib::cl::kernel_unique_ptr const splat_volume_float_;
};
}
}

#endif

