#ifndef FLAKELIB_PLANAR_SIMULATION_BASE_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_BASE_HPP_INCLUDED

#include <flakelib/duration.hpp>
#include <flakelib/buffer/planar_view.hpp>
#include <flakelib/planar/additional_scalar_data.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace planar
{
namespace simulation
{
class base
{
FCPPT_NONCOPYABLE(
	base);
public:
	virtual ~base() = 0;

	virtual void
	update(
		flakelib::duration const &) = 0;

	virtual flakelib::buffer::planar_view<cl_float2> const
	velocity() = 0;

	virtual
	flakelib::planar::additional_scalar_data const &
	additional_scalar_data() const = 0;
protected:
	explicit
	base();
};
}
}
}

#endif
