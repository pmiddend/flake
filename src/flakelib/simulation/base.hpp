#ifndef FLAKELIB_SIMULATION_BASE_HPP_INCLUDED
#define FLAKELIB_SIMULATION_BASE_HPP_INCLUDED

#include <flakelib/duration.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
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

	virtual sge::opencl::memory_object::image::planar &
	vector_field() = 0;
protected:
	explicit
	base();
};
}
}

#endif
