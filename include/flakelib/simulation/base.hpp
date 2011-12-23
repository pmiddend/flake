#ifndef FLAKELIB_SIMULATION_BASE_HPP_INCLUDED
#define FLAKELIB_SIMULATION_BASE_HPP_INCLUDED

#include <flakelib/additional_planar_data.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer/planar_view.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
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
	flakelib::additional_planar_data const &
	additional_planar_data() const = 0;
protected:
	explicit
	base();
};
}
}

#endif
