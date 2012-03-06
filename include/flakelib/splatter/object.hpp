#ifndef FLAKELIB_SPLATTER_OBJECT_HPP_INCLUDED
#define FLAKELIB_SPLATTER_OBJECT_HPP_INCLUDED

#include <flakelib/cl/float2.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/cl/unique_kernel_ptr.hpp>
#include <flakelib/planar/float2_view.hpp>
#include <flakelib/planar/float_view.hpp>
#include <flakelib/splatter/circle/object_fwd.hpp>
#include <flakelib/splatter/rectangle/object_fwd.hpp>
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
	explicit
	object(
		cl::program_context const &);

	void
	splat_float_circle(
		planar::float_view const &,
		splatter::circle::object const &,
		cl_float);

	void
	splat_float2_circle(
		planar::float2_view const &,
		splatter::circle::object const &,
		cl::float2 const &);

	void
	splat_float_rectangle(
		planar::float_view const &,
		splatter::rectangle::object const &,
		cl_float);

	void
	splat_float2_rectangle(
		planar::float2_view const &,
		splatter::rectangle::object const &,
		cl::float2 const &);

	/*
	void
	splat_float_sphere(
		volume::float_view const &,
		splat::sphere const &,
		cl_float);

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

	~object();
private:
	flakelib::cl::program program_;
	flakelib::cl::unique_kernel_ptr splat_float_circle_;
	flakelib::cl::unique_kernel_ptr splat_float2_circle_;
	flakelib::cl::unique_kernel_ptr splat_float_rectangle_;
	flakelib::cl::unique_kernel_ptr splat_float2_rectangle_;
};
}
}

#endif

